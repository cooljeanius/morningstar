//
// codename Morning Star
//
// Copyright (C) 2008 - 2019 by Iris Morelle <shadowm2006@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "appconfig.hpp"
#include "defs.hpp"
#include "custompalettes.hpp"
#include "customranges.hpp"
#include "mainwindow.hpp"
#include "paletteitem.hpp"
#include "rc_qt4.hpp"
#include "ui_mainwindow.h"
#include "util.hpp"
#include "version.hpp"

#include <QActionGroup>
#include <QColorDialog>
#include <QDesktopServices>
#include <QDrag>
#include <QFileDialog>
#include <QIcon>
#include <QImageReader>
#include <QImageWriter>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QScrollBar>
#include <QStandardPaths>
#include <QStyle>
#include <QUrl>
#include <QWhatsThis>

namespace {
	struct no_initial_file {};
	struct canceled_job    {};
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),

	color_ranges_(),
	palettes_(),

	user_color_ranges_(),
	user_palettes_(),

	ui(new Ui::MainWindow),

	img_path_(),
	img_original_(),
	img_transview_(),
	zoom_(1.0),
	ignore_drops_(false),
	drag_use_rc_(false),
	drag_start_(false),
	drag_start_pos_(),
	recent_file_acts_(),
	zoom_factors_({ 0.5, 1.0, 2.0, 4.0, 8.0 })
{
    ui->setupUi(this);

	mos_config_load(user_color_ranges_, user_palettes_);

	const QSize& lastWindowSize = mos_get_main_window_size();

	if(lastWindowSize.isValid()) {
		resize(lastWindowSize);
	}

	generateMergedRcDefinitions();
	processRcDefinitions();

	QAction* const act_whatsthis = QWhatsThis::createAction(this);
	ui->menu_Help->insertAction(ui->actionAbout_Morning_Star, act_whatsthis);
	ui->menu_Help->insertSeparator(ui->actionAbout_Morning_Star);

	QPushButton* const save = ui->buttonBox->button(QDialogButtonBox::Save);
	QPushButton* const close = ui->buttonBox->button(QDialogButtonBox::Close);

	save->setWhatsThis(tr("Saves the current recolor job."));
	close->setWhatsThis(tr("Abandons the current job and exits."));

#ifdef Q_OS_MACOS
	ui->action_Reload->setShortcut(QKeySequence::Refresh);
#endif

	ui->action_Open->setIcon(this->style()->standardIcon(QStyle::SP_DialogOpenButton, nullptr, dynamic_cast<QWidget*>(ui->action_Open)));
	ui->action_Save->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton, nullptr, dynamic_cast<QWidget*>(ui->action_Save)));
	ui->action_Reload->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload, nullptr, dynamic_cast<QWidget*>(ui->action_Reload)));
	ui->action_Quit->setIcon(this->style()->standardIcon(QStyle::SP_DialogCloseButton, nullptr, dynamic_cast<QWidget*>(ui->action_Quit)));

	// Use theme icons for some buttons on X11. This is not
	// in the .ui file for Qt 4.6 - 4.7 compatibility.

	ui->tbZoomIn->setIcon(QIcon::fromTheme("zoom-in", QIcon(":/zoom-in-16.png")));
	ui->tbZoomOut->setIcon(QIcon::fromTheme("zoom-out", QIcon(":/zoom-out-16.png")));

	for(unsigned k = 0; k < mos_max_recent_files(); ++k) {
		QAction* act = new QAction(this);

		act->setVisible(false);

		connect(act, SIGNAL(triggered()), this, SLOT(handleRecent()));

		ui->menu_File->insertAction(ui->action_RecentPlaceholder, act);
		recent_file_acts_.push_back(act);
	}

	ui->action_RecentPlaceholder->setVisible(false);

	update_recent_files_menu();

	const QString& bgColorName = mos_get_preview_background_color_name();

	QActionGroup* bgColorActs = new QActionGroup(this);

	// The Custom Color entry goes into the list first so that it is preemptively
	// selected first on the next loop in case the color saved in preferences isn't
	// any of the predefined ones.
	bgColorActs->addAction(ui->actionPreviewBgCustom);
	ui->actionPreviewBgCustom->setData(bgColorName);
	do_custom_preview_color_icon();
	bgColorActs->addAction(ui->actionPreviewBgBlack);
	ui->actionPreviewBgBlack->setData(QColor(Qt::black).name());
	bgColorActs->addAction(ui->actionPreviewBgDark);
	ui->actionPreviewBgDark->setData(QColor(Qt::darkGray).name());
	bgColorActs->addAction(ui->actionPreviewBgDefault);
	ui->actionPreviewBgDefault->setData("");
	bgColorActs->addAction(ui->actionPreviewBgLight);
	ui->actionPreviewBgLight->setData(QColor(Qt::lightGray).name());
	bgColorActs->addAction(ui->actionPreviewBgWhite);
	ui->actionPreviewBgWhite->setData(QColor(Qt::white).name());

	QList<QAction*> bgColorActList = bgColorActs->actions();
	for(auto* act : bgColorActList) {
		connect(act, SIGNAL(triggered(bool)), this, SLOT(handlePreviewBgOption(bool)));

		// We must find the menu item for the color we read from the app
		// config and mark it as checked, and update the preview background
		// color manually since setChecked() won't raise the triggered()
		// signal.
		if(act->data().toString() == bgColorName) {
			act->setChecked(true);
			setPreviewBackgroundColor(bgColorName);
		}
	}

	ui->radRc->setChecked(true);
	ui->staFunctionOpts->setCurrentIndex(0);
	toggle_page2(false);
	toggle_page1(true);

	ui->zoomSlider->setMinimum(0);
	ui->zoomSlider->setMaximum(zoom_factors_.size() - 1);
	ui->zoomSlider->setValue(1);

	ui->previewOriginalContainer->viewport()->setBackgroundRole(QPalette::Dark);
	ui->previewRcContainer->viewport()->setBackgroundRole(QPalette::Dark);

	//
	// FIXME: hack to prevent Oxygen stealing our drag events when dragging
	// windows from empty areas is enabled.
	//
	// http://lists.kde.org/?l=kde-devel&m=130530904703913&w=2
	//
	// We should probably figure out a better way to do this later, as well
	// as the preview panels themselves; the proper way according to the
	// Oxygen dev is to prevent (at the widget level) propagation of the event
	// to the window widget.
	//

	ui->previewOriginalContainer->setProperty("_kde_no_window_grab", true);
	ui->previewRcContainer->setProperty("_kde_no_window_grab", true);

	connect(
		ui->previewOriginalContainer->horizontalScrollBar(), SIGNAL(valueChanged(int)),
		ui->previewRcContainer->horizontalScrollBar(), SLOT(setValue(int)));
	connect(
		ui->previewRcContainer->horizontalScrollBar(), SIGNAL(valueChanged(int)),
		ui->previewOriginalContainer->horizontalScrollBar(), SLOT(setValue(int)));
	connect(
		ui->previewOriginalContainer->verticalScrollBar(), SIGNAL(valueChanged(int)),
		ui->previewRcContainer->verticalScrollBar(), SLOT(setValue(int)));
	connect(
		ui->previewRcContainer->verticalScrollBar(), SIGNAL(valueChanged(int)),
		ui->previewOriginalContainer->verticalScrollBar(), SLOT(setValue(int)));

	enableWorkArea(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::generateMergedRcDefinitions()
{
	color_ranges_ = mosBuiltinColorRanges;
	color_ranges_.insert(user_color_ranges_);
	palettes_ = mosBuiltinColorPalettes;
	palettes_.insert(user_palettes_);
}

void MainWindow::insertRangeListItem(const QString &id, const QString &display_name)
{
	QListWidgetItem* lwi = new QListWidgetItem(ui->listRanges);

	lwi->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	lwi->setCheckState(Qt::Checked);
	lwi->setText(display_name);
	lwi->setData(Qt::UserRole, id);
}

void MainWindow::processRcDefinitions()
{
	QComboBox* cbOldPals = ui->cbxKeyPal;
	QComboBox* cbNewPals = ui->cbxNewPal;

	cbOldPals->clear();
	cbNewPals->clear();
	ui->listRanges->clear();

	//
	// We allow built-in definitions to be overridden by the user
	// by defining their own ranges/palettes with mainline ids; when
	// that happens, their data is internally merged, but these
	// overridden definitions continue to be displayed before regular
	// user-defined items.
	//
	// This makes the process of entering those definitions for the
	// UI less trivial than it should be. Additionally, we keep only
	// ids for user-defined data and use our own translatable names
	// for displaying built-ins, so we must attach extra data to item
	// widget entries to ensure things look good in the front-end.
	//

	//
	// Add built-in palettes.
	//

	QStringList paletteUiNames;
	// NOTE: these names must correspond to the entries in mosOrderedPaletteNames!
	paletteUiNames << tr("Magenta TC") << tr("Green flag TC") << tr("Red ellipse TC");

	const int builtinPaletteCount = paletteUiNames.size();
	Q_ASSERT(builtinPaletteCount == mosOrderedPaletteNames.size());

	for(int k = 0; k < builtinPaletteCount; ++k) {
		cbOldPals->addItem(paletteUiNames[k], mosOrderedPaletteNames[k]);
		cbNewPals->addItem(paletteUiNames[k], mosOrderedPaletteNames[k]);
	}

	//
	// User-defined palettes.
	//

	const auto& userPaletteIds = user_palettes_.keys();
	for(const auto& pal_name : userPaletteIds) {
		if(mosBuiltinColorPalettes.find(pal_name) != mosBuiltinColorPalettes.end()) {
			// Skip redefinitions of built-in palettes, we only care about
			// ids and names at this point.
			continue;
		}
		cbOldPals->addItem(capitalize(pal_name), pal_name);
		cbNewPals->addItem(capitalize(pal_name), pal_name);
	}

	//
	// Built-in color ranges.
	//

	// NOTE: these names must correspond to the entries in mosOrderedRangeNames!
	QStringList rangeUiNames = {
		tr("Red"), tr("Blue"), tr("Green"),
		tr("Purple"), tr("Black"), tr("Brown"),
		tr("Orange"), tr("White"), tr("Teal"),
		tr("Light Red"), tr("Dark Red"), tr("Light Blue"),
		tr("Bright Green"), tr("Bright Orange"), tr("Gold"), /*
		tr("Terrain Icon: Reef"),
		tr("Terrain Icon: Shallow Water"),
		tr("Terrain Icon: Deep Water"),
		tr("Terrain Icon: Swamp Water"),
		tr("Terrain Icon: Flat"),
		tr("Terrain Icon: Hills"),
		tr("Terrain Icon: Mountains"),
		tr("Terrain Icon: Forest"),
		tr("Terrain Icon: Sand"),
		tr("Terrain Icon: Frozen"),
		tr("Terrain Icon: Cave"),
		tr("Terrain Icon: Fungus"),
		tr("Terrain Icon: Village"),
		tr("Terrain Icon: Castle"),
		tr("Terrain Icon: Keep"),
		tr("Terrain Icon: Rail"),
		tr("Terrain Icon: Unwalkable"),
		tr("Terrain Icon: Impassable"),
		tr("Terrain Icon: Fog"),
		tr("Terrain Icon: Shroud"), */
	};

	// It is paramount to ensure built-in ranges are displayed in a specific order,
	// since Wesnoth associates digits from 1 to 9 to items in the sequence and we
	// don't want to break that convention here, for consistency's sake.

	const int builtinRangeCount = rangeUiNames.size();
	Q_ASSERT(builtinRangeCount == mosOrderedRangeNames.size());

	for(int k = 0; k < builtinRangeCount; ++k) {
		insertRangeListItem(mosOrderedRangeNames[k], rangeUiNames[k]);
	}

	//
	// User-defined color ranges
	//

	const auto& userRangeIds = user_color_ranges_.keys();
	for(const auto& id : userRangeIds) {
		if(mosBuiltinColorRanges.find(id) != mosBuiltinColorRanges.end()) {
			// Skip redefinitions of built-in ranges, we only care about
			// ids and names at this point.
			continue;
		}
		insertRangeListItem(id, capitalize(id));
	}

	ui->listRanges->setCurrentRow(0);
	ui->staWorkAreaParent->setCurrentIndex(1);
}

void MainWindow::handleRecent()
{
	QAction* act = qobject_cast<QAction*>(sender());
	if(act) {
		this->do_open(act->data().toString());
	}
}

void MainWindow::update_recent_files_menu()
{
	const QStringList& recent = mos_recent_files();

	for(int k = 0; k < recent_file_acts_.size(); ++k) {
		QAction& act = *recent_file_acts_[k];
		if(k < recent.size()) {
			act.setText(QString("&%1 %2").arg(k + 1).arg(QFileInfo(recent[k]).fileName()));
			act.setData(recent[k]);
			act.setEnabled(true);
			act.setVisible(true);
		} else {
			act.setEnabled(false);
			act.setVisible(false);
		}
	}
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent * /*e*/)
{
	mos_set_main_window_size(size());
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	if(event->matches(QKeySequence::ZoomIn)) {
		on_tbZoomIn_clicked();
	} else if(event->matches(QKeySequence::ZoomOut)) {
		on_tbZoomOut_clicked();
	} else {
		QMainWindow::keyPressEvent(event);
	}
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
	if(event->angleDelta().x() == 0 && event->modifiers() & Qt::ControlModifier)
	{
		if(event->angleDelta().y() > 0) {
			on_tbZoomIn_clicked();
		} else if(event->angleDelta().y() < 0) {
			on_tbZoomOut_clicked();
		}
		event->accept();
		return;
	}
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton && !img_original_.isNull() && !img_transview_.isNull()) {
		this->drag_start_pos_ = event->pos();
		this->drag_use_rc_ = ui->previewRcContainer->geometry().contains(event->pos());
		this->drag_start_ = drag_use_rc_ || ui->previewOriginalContainer->geometry().contains(event->pos());
	}
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	if(drag_start_ && (event->buttons() & Qt::LeftButton) && (event->pos() - drag_start_pos_).manhattanLength() >= QApplication::startDragDistance()) {

		QDrag *d = new QDrag(this);
		QMimeData *m = new QMimeData();

		if(drag_use_rc_)
			m->setImageData(this->img_transview_);
		else
			m->setImageData(this->img_original_);

		d->setMimeData(m);

		ignore_drops_ = true;
		d->exec(Qt::CopyAction);
		ignore_drops_ = drag_start_ = false;
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
	if(e->mimeData()->hasImage() || e->mimeData()->hasUrls())
		e->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *e)
{
	if(ignore_drops_)
		return;

	QImage newimg;
	QString newpath = "";

	e->acceptProposedAction();

	if(e->mimeData()->hasImage()) {
		newimg = qvariant_cast<QImage>(e->mimeData()->imageData());
	}
	else if(e->mimeData()->hasUrls()) {
		newpath = e->mimeData()->urls().front().path();
		newimg.load(newpath);
	}

	if(newimg.isNull()) {
		return;
	}

	img_original_ = newimg.convertToFormat(QImage::Format_ARGB32);

	// Refresh UI
	if(newpath.isEmpty() != true) {
		img_path_ = newpath;
		update_window_title(img_path_);
	}
	else {
		update_window_title("");
	}

	refresh_previews();
}

void MainWindow::on_radRc_clicked()
{
	ui->staFunctionOpts->setCurrentIndex(0);
	toggle_page1(true);
	toggle_page2(false);
	refresh_previews();
}

void MainWindow::on_radPal_clicked()
{
	ui->staFunctionOpts->setCurrentIndex(1);
	toggle_page1(false);
	toggle_page2(true);
	refresh_previews();
}

void MainWindow::toggle_page1(bool newstate)
{
	ui->listRanges->setEnabled(newstate);
}

void MainWindow::toggle_page2(bool newstate)
{
	ui->cbxNewPal->setEnabled(newstate);
	ui->lblNewPal->setEnabled(newstate);
}

void MainWindow::on_actionAbout_Morning_Star_triggered()
{
	do_about();
}

void MainWindow::on_buttonBox_clicked(QAbstractButton* button)
{
	QDialogButtonBox::StandardButton btype = ui->buttonBox->standardButton(button);

	switch(btype) {
	case QDialogButtonBox::Save:
		do_save();
		break;
	case QDialogButtonBox::Close:
		do_close();
		break;
	default:
		;
	}
}

void MainWindow::on_action_Open_triggered()
{
	do_open();
}

void MainWindow::on_action_Quit_triggered()
{
	do_close();
	this->close();
}

void MainWindow::on_action_Reload_triggered()
{
	do_reload();
}

bool MainWindow::initial_open(const QString &initial_file)
{
	try {
		this->do_open(initial_file);
	}
	catch(no_initial_file const&) {
		return false;
	}

	return true;
}

QString MainWindow::supported_file_patterns() const
{
	QString ret;

	QMap<QString, QString> all_fmts = {
		{ "*.png", tr("PNG image") },
		{ "*.bmp", tr("Windows Bitmap") },
	};

	QMap<QString, QString> optional_fmts = {
		{ "*.xcf", tr("GIMP image") },
		{ "*.psd", tr("Photoshop image") },
		{ "*.ora", tr("OpenRaster image") },
		{ "*.kra", tr("Krita image") },
	};

	QList<QByteArray> supported_list = QImageReader::supportedImageFormats();
	for (const auto& supp : supported_list) {
		QString str = "*." + supp;
		auto it = optional_fmts.find(str.toLower());
		if (it != optional_fmts.end()) {
			all_fmts[it.key()] = it.value();
		}
	}

	ret += tr("All Supported Files") + " (";
	ret += QStringList{all_fmts.keys()}.join(' ') + ")";

	for (const auto& ext : all_fmts.keys()) {
		ret += ";;" + all_fmts[ext] + " (" + ext + ")";
	}

	ret += ";;" + tr("All Files") + " (*)";

	return ret;
}

void MainWindow::do_open(const QString &initial_file)
{
	QString path_temp;
	QString start_dir;

	QStringList picture_locations =
			QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);

	if(initial_file.isNull() || initial_file.isEmpty()) {
		if(img_path_.isEmpty()) {
			if(!picture_locations.empty()) {
				start_dir = picture_locations.first();
			} else {
				start_dir = ".";
			}
		} else {
			start_dir = QFileInfo(img_path_).absolutePath();
		}

		path_temp = QFileDialog::getOpenFileName(
			this,
			tr("Choose source image"),
			start_dir,
			supported_file_patterns()
		);
	}
	else {
		path_temp = initial_file;
	}

	if(path_temp.isNull() && img_original_.isNull()) {
		// it's null if we've just setup the window
		throw no_initial_file();
	}

	QImage img_temp(path_temp);
	if(img_temp.isNull()) {
		if(path_temp.isNull() != true) {
			MosUi::error(
				this, tr("Could not load %1.").arg(path_temp));
		}

		if(img_original_.isNull()) {
			throw no_initial_file();
		}

		mos_remove_recent_file(path_temp);

		return;
	}

	img_path_ = path_temp;
	// We want to work on actual ARGB data
	img_original_ = img_temp.convertToFormat(QImage::Format_ARGB32);

	// Refresh UI
	mos_add_recent_file(img_path_);
	update_recent_files_menu();
	update_window_title(img_path_);
	refresh_previews();

	enableWorkArea(true);
}

void MainWindow::do_reload()
{
	QImage img(img_path_);
	if(img.isNull()) {
		MosUi::error(this, tr("Could not reload %1.").arg(img_path_));
		return;
	}

	img_original_ = img.convertToFormat(QImage::Format_ARGB32);

	// Refresh UI
	refresh_previews();
}

void MainWindow::refresh_previews()
{
	if(this->img_original_.isNull() || this->signalsBlocked())
		return;

	rc_map cvtMap;
	const QList<QRgb>& palData = current_pal_data();

	if(ui->staFunctionOpts->currentIndex()) {
		const QList<QRgb>& targetPalData = current_pal_data(true);
		cvtMap = recolor_palettes(palData, targetPalData);
	} else {
		cvtMap = recolor_range(color_ranges_.value(ui->listRanges->currentIndex().data(Qt::UserRole).toString()), palData);
	}

	rc_image(img_original_, img_transview_, cvtMap);

	const QSize& scaled_size = img_original_.size() * zoom_;

	ui->previewOriginal->setPixmap(QPixmap::fromImage(img_original_));
	ui->previewRc->setPixmap(QPixmap::fromImage(img_transview_));

	ui->previewOriginal->resize(scaled_size);
	ui->previewRc->resize(scaled_size);

	ui->previewOriginal->parentWidget()->adjustSize();
	ui->previewRc->parentWidget()->adjustSize();

	centerScrollArea(ui->previewOriginalContainer);
	centerScrollArea(ui->previewRcContainer);

	ui->staWorkAreaParent->setCurrentIndex(0);
}

void MainWindow::centerScrollArea(QScrollArea *scrollArea)
{
	if(!scrollArea || !scrollArea->widget())
		return;

	const QSize& childSize = scrollArea->widget()->size();
	const QSize& viewSize = scrollArea->viewport()->size();

	scrollArea->ensureVisible(childSize.width()/2, childSize.height()/2, viewSize.width()/2, viewSize.height()/2);
}

void MainWindow::do_save()
{
	QString base = QFileDialog::getExistingDirectory(
		this,
		tr("Choose an output directory"),
		QFileInfo(img_path_).absolutePath(),
		QFileDialog::ShowDirsOnly
	);

	if(base.isNull()) {
		return;
	}

	QStringList succeeded;

	try {
		if(ui->staFunctionOpts->currentIndex()) {
			succeeded = do_save_single_recolor(base);
		}
		else {
			succeeded = do_save_color_ranges(base);
		}

		MosUi::message(this, tr("The output files have been saved successfully."), succeeded);
	} catch(const canceled_job&) {
		;
	} catch(const QStringList& failed) {
		MosUi::error(this, tr("Some files could not be saved correctly."), failed);
	}
}

void MainWindow::do_close()
{
	enableWorkArea(false);
	this->img_original_ = QImage{};
	this->img_transview_ = QImage{};
}

void MainWindow::do_about()
{
	MosUi::about(this);
}

void MainWindow::enableWorkArea(bool enable)
{
	if (!enable) {
		this->setWindowTitle(tr("Wesnoth RCX"));
	}

	auto elements = std::make_tuple(
		ui->radPal, ui->radRc,
		ui->lblKeyPal, ui->cbxKeyPal,
		ui->lblNewPal, ui->cbxNewPal,
		ui->listRanges,
		ui->zoomSlider, ui->tbZoomIn, ui->tbZoomOut,
		ui->buttonBox->button(QDialogButtonBox::Save),
		ui->buttonBox->button(QDialogButtonBox::Close));

	std::apply([enable](auto&&... widget) {
		(widget->setEnabled(enable), ...);
	}, elements);

	ui->staWorkAreaParent->setCurrentIndex(enable ? 0 : 1);
}

QString MainWindow::current_pal_name(bool palette_switch_mode) const
{
	QComboBox* combow = palette_switch_mode ? ui->cbxNewPal : ui->cbxKeyPal;

	const int choice = combow->currentIndex();
	const QString& palette_name = combow->itemData(choice).toString();

	Q_ASSERT(!palette_name.isEmpty());

	return palette_name;
}

QList<QRgb> MainWindow::current_pal_data(bool palette_switch_mode) const
{
	return palettes_.value(current_pal_name(palette_switch_mode));
}

bool MainWindow::confirm_existing_files(const QStringList& paths)
{
	return MosUi::prompt(this, tr("The chosen directory already contains files with the same names required for output. Do you wish to overwrite them and continue?"), paths);
}

QStringList MainWindow::do_save_single_recolor(QString &base)
{
	QMap<QString, rc_map> jobs;

	const QString& palId = current_pal_name();
	const QList<QRgb>& palData = current_pal_data();

	const QString& targetPalId = current_pal_name(true);
	const QList<QRgb>& targetPalData = current_pal_data(true);

	const QString& filePath = base + "/" + QFileInfo(img_path_).completeBaseName() +
			"-PAL-" + palId + "-" + targetPalId + ".png";

	jobs[filePath] = recolor_palettes(palData, targetPalData);

	if(QFileInfo(filePath).exists() && !confirm_existing_files(QStringList(filePath))) {
		throw canceled_job();
	}

	return do_run_jobs(jobs);
}

QStringList MainWindow::do_save_color_ranges(QString &base)
{
	QMap<QString, rc_map> jobs;

	const QString& palId = current_pal_name();
	const QList<QRgb>& palData = current_pal_data();

	QStringList needOverwriteFiles;

	for(int k = 0; k < ui->listRanges->count(); ++k) {
		QListWidgetItem* itemw = ui->listRanges->item(k);
		Q_ASSERT(itemw);

		if(itemw->checkState() == Qt::Checked) {
			const QString& rangeId = itemw->data(Qt::UserRole).toString();

			const QString& filePath = base + "/" + QFileInfo(img_path_).completeBaseName() +
					"-RC-" + palId + "-" + QString::number(k + 1) +
					"-" + rangeId + ".png";

			jobs[filePath] = recolor_range(color_ranges_.value(rangeId), palData);

			if(QFileInfo(filePath).exists()) {
				needOverwriteFiles.push_back(filePath);
			}
		}
	}

	if(!needOverwriteFiles.isEmpty() && !confirm_existing_files(needOverwriteFiles)) {
		throw canceled_job();
	}

	return do_run_jobs(jobs);
}

QStringList MainWindow::do_run_jobs(QMap<QString, rc_map> &jobs)
{
	QStringList failed, succeeded;
	QString propaganda = QString("Generated by Morning Star v%1").arg(mos_version);
	this->setEnabled(false);

	for(QMap<QString, rc_map>::const_iterator k = jobs.begin(); k != jobs.end(); ++k) {
		QImage rc;
		QImageWriter out;
		out.setFormat("png");
		out.setFileName(k.key());
		out.setText("", propaganda);

		if(rc_image(this->img_original_, rc, k.value()) && out.write(rc)) {
			succeeded.push_back(out.fileName());
		}
		else {
			failed.push_back(out.fileName());
		}
	}

	this->setEnabled(true);

	if(failed.isEmpty() != true) {
		throw failed;
	}

	return succeeded;
}

void MainWindow::on_action_Save_triggered()
{
	do_save();
}

void MainWindow::on_cbxKeyPal_currentIndexChanged(int /*index*/)
{
	refresh_previews();
}

void MainWindow::on_cbxNewPal_currentIndexChanged(int /*index*/)
{
	refresh_previews();
}

void MainWindow::on_listRanges_currentRowChanged(int /*currentRow*/)
{
	refresh_previews();
}


void MainWindow::on_zoomSlider_valueChanged(int value)
{
	Q_ASSERT(value >= 0 && value < zoom_factors_.size());
	zoom_ = zoom_factors_[value];

	update_zoom_buttons();
	refresh_previews();
}

void MainWindow::on_tbZoomIn_clicked()
{
	ui->zoomSlider->setValue(ui->zoomSlider->value() + 1);
}

void MainWindow::on_tbZoomOut_clicked()
{
	ui->zoomSlider->setValue(ui->zoomSlider->value() - 1);
}

void MainWindow::update_zoom_buttons()
{
	QSlider& zoomSlider= *ui->zoomSlider;

	ui->tbZoomOut->setEnabled(zoomSlider.value() != zoomSlider.minimum());
	ui->tbZoomIn->setEnabled(zoomSlider.value() != zoomSlider.maximum());
}

void MainWindow::on_actionColor_ranges_triggered()
{
	CustomRanges dlg(user_color_ranges_, this);
	dlg.exec();

	if(dlg.result() == QDialog::Rejected)
		return;

	user_color_ranges_ = dlg.ranges();

	{
		ObjectLock l(this);
		generateMergedRcDefinitions();
		processRcDefinitions();
	}

	refresh_previews();

	mos_config_save(user_color_ranges_, user_palettes_);
}

void MainWindow::on_action_Palettes_triggered()
{
	CustomPalettes dlg(user_palettes_, color_ranges_, this);
	dlg.exec();

	if(dlg.result() == QDialog::Rejected)
		return;

	user_palettes_ = dlg.getPalettes();

	{
		ObjectLock l(this);
		generateMergedRcDefinitions();
		processRcDefinitions();
	}

	refresh_previews();

	mos_config_save(user_color_ranges_, user_palettes_);
}

void MainWindow::handlePreviewBgOption(bool checked)
{
	if(!checked)
		return;

	QAction* const act = qobject_cast<QAction*>(sender());

	if(!act)
		return;

	if(act == ui->actionPreviewBgCustom) {
		// We want to give the user the option to customize the preview color
		// first.
		do_custom_preview_color_option();
	}

	setPreviewBackgroundColor(act->data().toString());
}

void MainWindow::do_custom_preview_color_option()
{
	QAction* const act = ui->actionPreviewBgCustom;
	QColor userColor = QColorDialog::getColor(QColor(act->data().toString()), this);
	if(userColor.isValid()) {
		act->setData(userColor.name());
		do_custom_preview_color_icon();
	}
}

void MainWindow::do_custom_preview_color_icon()
{
	QAction* const act = ui->actionPreviewBgCustom;
	createColorIcon(QColor(act->data().toString()));
}

void MainWindow::setPreviewBackgroundColor(const QString& colorName)
{
	const QString ss = "* { background-color: " + colorName + "; }";

	ui->previewOriginalContainer->viewport()->setStyleSheet(ss);
	ui->previewRcContainer->viewport()->setStyleSheet(ss);

	mos_set_preview_background_color_name(colorName);
}

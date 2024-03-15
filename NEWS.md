Changelog
=========

Version 0.4.0
-------------

* It is now possible to close the active file without quitting the app.
* Not opening a file on startup no longer causes the app to quit.
* Various fixes for Qt 6 compatibility.
* Various changes to make the app feel more native on macOS.

Version 0.3.0
-------------

* Ported to Qt 5 and C++11.
* Added more unit team colors introduced in Wesnoth 1.14 (light red, dark
   red, light blue, bright green, bright orange, gold).
* Palette Editor: Update palette icon after recoloring.
* Main: Zoom view in/out using Ctrl++ (plus) or Ctrl+- (minus).
* Main: Zoom view in/out using Ctrl+<vertical scrollwheel>.
* Main: Made it possible for the user to choose any custom color for the
  preview background.

Version 0.2.1
-------------

* Compatibility fixes for Qt 4.6 and 4.7.
* Add Colors dialog: Minor visual improvements
* Main: Make preview background color setting persistent.
* Main: Remember the main window size for subsequent runs.
* Main: Solve excessive memory usage when zooming in. This can save around
  1 GiB of RAM in some cases (!).
* Palette Editor: "Add from List" should stay disabled when there are no
  available palettes.
* Palette Editor: Added option to apply a color range (team coloring) to
  the current selected palette.

Version 0.2.0
-------------

* Added an embedded application icon and version information to Windows
  builds.
* Added various preview background color options.
* Implemented built-in editors for custom color ranges and palettes,
  as well as options to export individual items as WML (either to a
  new file, or the system clipboard).
* Implemented Reload (F5) action.
* Implemented Recent Files list in the File menu.
* Redesigned the main window.

Version 0.1.4
-------------

* Complete drag-and-drop support.
* Implemented basic zoom support.

Version 0.1.3
-------------

* Fix an off-by-one bug causing problems with the TC recolor jobs.


Version 0.1.2
-------------

* Added support for BMP, PSD* and XCF* files (* indicates an option that
  depends on Qt4's build-time configuration.)
* Drop support for image files and data from other applications.
* Minor UI improvements or revisions.


Version 0.1.1
-------------

* Preliminary production name: “Wesnoth RCX” (a.k.a. RCX).
* Made the main window resizable again — now widgets expand or shrink
  accordingly.
* Added generated file paths to completed job reports.
* Fixed some glitches with selection changes in the color ranges listbox.
* Minor UI improvements or revisions.

Version 0.1.0.1
---------------

* Fixed a bogus message when canceling the Open action.
* Made the window non-resizable since widgets don’t adjust to it yet.
* Previews are now centered.
* Correctly remember the path to the currently opened file.

Version 0.1.0
-------------

* Initial version.

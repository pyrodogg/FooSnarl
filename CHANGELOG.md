#FooSnarl Changelog
#### v2.0-beta
* Add preferences page. No more advanced config tree!
  * Includes live formatting preview panel and notification test button.
  * Formatting for title and body *should* be preserved. If you have anything super intricate, I recommend backing it up before upgrading.
* Add Now Playing menu item that can be bound to hotkey in foobar2000. Triggers notification of current playback state. (#7)
* Add notification actions.
  * Can be turned off in preferences.
* Remove timeout config. Default is 5 seconds can be overridden in Snarl
* Much code refactoring

#### v1.1.0
* 12/16/2010
* Notify on dynamic track changes (e.g. title changes)
* Embedded album art support

#### v1.0.0
* 12/12/2010
* One notification at a time.
* Sample actions (w/ Snarl 2.4b2 beta)
* Right-click notification to open Fb2k
* Removed notification on ___ options.  Can be controlled through Snarl message classes.
* Depricated default icon configuration.  Defaults to Fb2k icon.

####v0.2.0
* 12/8/2010
* Updated to latest SnarlInterface by Matt Battcher
* Built with Fb2k SDK 2010-10-02
* Added message classes (Play, Pause, Stop) to corresponding events.  Each class can be assigned it's own style in Snarl.
* Incorporated patch submitted by grey_teardrop to fix unicode filepath and cover art.
* Stop event reports the track that was playing.

####v0.1.6
* 8/11/08
* Dependency Problem fixed
* Default album art string fixed

####v0.1.5
* 8/10/08
* Actually does default to default image if album art is not present

####v0.1
* First Public Release

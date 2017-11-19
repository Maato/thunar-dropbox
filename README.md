# Thunar Dropbox

Thunar Dropbox is a plugin for Thunar that adds context-menu items from 
Dropbox. This plugin does not come with Dropbox itself, you will need to 
install that separately.

![Thunar-Dropbox](http://softwarebakery.com/maato/images/thunardropbox_contextmenu.png)

### Dependencies
Depending on your distribution you might have to install 
`libthunarx-3`.

### Installation from sources
    python2 ./waf configure
    python2 ./waf build
    sudo python2 ./waf install

_Note_: the provided waf does not work correctly with python3, so make
sure you use python2 to run these commands.

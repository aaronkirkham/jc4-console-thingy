## Just Cause 4 Console Thingy
Spawn anything and everything, vehicles, characters, animals, props. Enable infinite ammo, toggle hidden features.

<p align="center"><img src="https://i.imgur.com/pDelm9v.png" alt=""></p>

### How to Install
 - Download the latest version from [releases](https://github.com/aaronkirkham/jc4-console-thingy/releases)
 - Extract `xinput9_1_0.dll` to your Just Cause 4 installation folder
 - Start the game and press the tilde (\`~) or F1 key to open the console
 - ***OPTIONAL*** - Enable super fast game loading with the `-quickstart` launch argument via Steam

### Commands
 - `spawn` - Spawn everything! Vehicles, characters, animals & props.
 - `event` - Trigger game events - **CAUTION** - Stick to using only the hints, unless you know what the event does
 - `world` - Change world parameters
 - `skin` - Change player skin
 - `tp` - Teleport player to position
   - `tp <x> <y> <z>` - Go directly to coordinates
   - `tp save <name>` - Save current coordinates as named location
   - `tp <name>` - Go to saved named location
   - `tp delete <name>` - Delete saved named location

### Controls
 - `Tilde` (\`~) or `F1` - Toggle the input box
 - `Escape` - Clear current input text
 - `Up/Down Arrows` - Navigate the input history
 - `TAB` - Change focus to the hints list
 - **When hints list is focused**
   - `Escape` - Return focus to main input
   - `Up/Down Arrows` - Navigate the list
   - `Enter` - Append the current hint to the input text

#### Building
If you want to compile the code yourself, you will need **Visual Studio 2017 or later** (or a compiler which supports C++17 nested namespaces)
 - Clone this repository
 - Run `configure.ps1` with PowerShell
 - Build `out/jc4-console-thing.sln` in Visual Studio

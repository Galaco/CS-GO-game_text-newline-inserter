# Counterstrike: Global Offensive game_text newline inserter

Valve finally fixed game_text in CS:GO, including support for LF newlines in game_text messages. Unfortunately, they didn't update hammer, and newlines will corrupt the .vmf and crash hammer on opening.

### What does this tool do?
It replaces characters found withing bsp's entdata with a the newline character. Basically, instead of using newline in the vmf, use a character as a placeholder that you don't use elsewhere and won't make Hammer crash (e.g. '$').

### Usage
Run from command line (It will create a backup before making any changes): 

`CsgoGameTextNewLiner_x64.exe --target="<full_path_to_file/filename.bsp>" --placeholder="<placeholder_character>"`

You can also run it as a build step from Hammer using the above

### Todo
* Limit search to within Addoutput's and game_text message values. This would allow you to use any character you want so long as its unique within that key/value
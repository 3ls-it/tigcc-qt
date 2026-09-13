#### QScintilla

```text
startup empty state                     Good
open one file                           Good
open multiple files                     Good
activate existing files                 Good
move tabs                               Good
save active file                        Good
save inactive file via Save All         Good via Save|Discard|Cancel
discard active/inactive modified files  Good via Save|Discard|Cancel
close clean tab                         Good
attempt to close modified tab           Good
close all                               Good
quit Save/Discard/Cancel                Good
project transition Save/Discard/Cancel  Good
backend transition                      Good
font-size changes                       Good
```

#### Vim

```text
startup empty state                     Good
multiple independent Vim sessions       Good
activate existing session               Good
move tabs                               Good
:w                                      Good
File → Save                             Good
Ctrl+S                                  Good
Save All                                Good via Save|Discard|Cancel
Discard All                             Good via Save|Discard|Cancel
:q                                      Good
:wq                                     Good
close clean tab                         Good
attempt to close modified tab           Good
project transition                      Good
backend transition                      Good
temporary-file cleanup                  Good
font-size changes                       Good
```

#### KTextEditor

Run the same matrix as QScintilla:  

```text
startup empty state                     Good
open one file                           Good
open multiple files                     Good
activate existing files                 Good
move tabs                               Good
save active file                        Good
save inactive file via Save All         Good via Save|Discard|Cancel
discard active/inactive modified files  Good via Save|Discard|Cancel
close clean tab                         Good
attempt to close modified tab           Good
close all                               Good
quit Save/Discard/Cancel                Good
project transition Save/Discard/Cancel  Good
backend transition                      Good
font-size changes                       Good
```

#### To Fix  
  
- In both Vim and KTextEditor, the tab file names are not marked as modified with an asterisk. The application title bar does show the asterisk for modified files.  
  
- There is no "Save All" option in the file menu, however triggering the "Save|Discard|Cancel" dialogue via a project change, backend change, or application exit _does_ properly save all modified files. Suggestion:  
    - The "Save Project" option in the "Project" menu currently saves only the project file after new files have been added. "Save Project" should also save all of the projects modified files. This is a reasonable user expectation.  
    - Since this change touches project lifecycle, we can include the feature in our planned expansion of the "Project" menu and project tree.

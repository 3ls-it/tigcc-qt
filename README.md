<div align="center">
<h2>TIGCC-Qt</h2>
</div>
&nbsp;
  
<div align="center">
  <img src="https://github.com/3ls-it/images/blob/main/default256.png" alt="TIGCC-Qt logo" resize="256"> 
</div>
&nbsp;
&nbsp;
<div align=center>
<h4>A modern, cross-platform Qt-based IDE for the GCC4TI toolchain. A successor to KTIGCC.</h4>
</div>
&nbsp;
&nbsp;
<p align="center">
This project is very new and the current code does not do much yet. Many things are bound to change (possibly a lot!) before we achieve a usable first release.
</p>

<p align="center">
For editor component integration, we have written a generic `EditorBackend` API. Currently there are three usable editor backends: QScintilla will be default for the release version and KTextEditor as a build-time option. The third editor option is Vim, which is contained by tabbed instances of QTermWidget.  
</p>
<p align="center">
Completion data has been extracted from the original KTIGCC completion file. We are using a JSON structure to hold generic, editor-neutral completion data, which is made available to the editors via a completion data backend class. Each editor backend has it's own corresponding completion data adapter. Syntax completion is fully functional for all three editors, but there will be some clean-up and detail word done soon.  
</p>
<p align="center">
To achieve reasonable search performance, we parse, sort and load the JSON data once at application startup. A binary search is used to find a matching completion entry, `O(log n)`, then a linear search of returned matches, `O(k)`. We should achieve efficiency of _approximately_ `O(log n + k)`, where `n` is the number of completion entries, and `k` is the number of returned hits. With the `7434` entries, there is no noticeable look up lag.
</p>
&nbsp;
&nbsp;
<h4>TIGCC-Qt with QScintilla with completion</h4>
&nbsp;
<div>
  <img src="https://github.com/3ls-it/tigcc-qt/blob/main/tigcc-qt_alpha-8.png" alt="TIGCC-Qt running QScintilla">
</div>
&nbsp;
&nbsp;
<h4>TIGCC-Qt with Vim in tabbed instances with completion</h4>
&nbsp;
<div>
  <img src="https://github.com/3ls-it/tigcc-qt/blob/main/tigcc-qt_alpha-7.png" alt="TIGCC-Qt running Vim">
</div>
&nbsp;
&nbsp;
<h4>TIGCC-Qt with optional KTextEditor and completion menu</h4>
&nbsp;
<div>
  <img src="https://github.com/3ls-it/tigcc-qt/blob/main/tigcc-qt_alpha-3.png" alt="TIGCC-Qt running KTextEditor">
</div>

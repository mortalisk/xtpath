xtpath
======

Expression templates for C++11, giving an xpath like syntax for parsing xml or similar hierarchical documents. Xtpath can wrap any DOM-like library, and can add namespace support if needed. An adaptor class is provided for pugi-xml. You can easily create adaptor classes for other popular xml-parsers such as Xerces, or your own hierarchical document type and parser.

Example
-------
To start, you first have to wrap a regular node in a Context-node:
```c++
auto doc = context(node);
```
You can then start querying the document from this context node. Say you want to find a node "bird", which has a child node "appearance" with an attribute "color" set to "black":
```c++
auto black_bird = doc | child("bird") | where(child("appearance") | attribute("color", "black"))
```

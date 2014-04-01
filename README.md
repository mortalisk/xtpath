xtpath
======
XML access with namespaces in a clear and concise syntax for c++11.

Example
-------
We would like to parse the following xml:
```xml
<collection>
  <bird>
    <name>Raven</name>
    <appearance color="black" size="medium">
  </bird>
  <bird>
    <name>Albatross</name>
    <appearance color="white" size="big">
  </bird>
  <bird>
    <name>Blackbird</name>
    <appearance color="black" size="small">
  </bird>
  <bird>
    <name>Norwegian Blue</name>
    <appearance color="blue" size="medium">
  </bird>
</collection>
```
To start, you first have to wrap a regular node (e.g. a pugi::xml_node) in a Context-node:
```c++
auto doc = context(node);
```
You can then start querying the document from this context node. Say you want to find the name of all birds with the color black:
```c++
auto black_bird = doc | child("bird") | 
                          where( child("appearance") | attribute("color", "black") ) |
                        child("name") | text;
```
You may now iterate through the results:
```c++
for(std::string name: black_birds)
    cout << name << "is a black bird\n";
```
If you want to pass around the result ranges, you can use the range<T> template to avoid the long types of the actual ranges:
```c++
int count_blue(range<_context<PugiXmlAdaptor> > & all_birds) {
  return boost::distance(all_birds | where( child("appearance") | attribute("color", "blue") ) );
}

void foo(range<_context<PugiXmlAdaptor> & collection) {
  auto all_birds = collection | child("bird");
  cout << "There are " << count_blue(all_birds) << " blue birds\n";
}

```

Some Advantages
---------------
1. Add support for namespaces where the underlying parser/DOM does not support it
2. Make it easy to switch the underlying parser/DOM.
3. Make your code easier to read.
4. Type safe and efficient.

Supported parsers
-----------------
Comes with Adaptor class for pugixml. You can easily create your own Adaptor classes for other parsers.

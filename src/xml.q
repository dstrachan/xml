/////////////
// PRIVATE //
/////////////

.xml.priv.parseFile:(` sv(.utl.PKGSLOADED"xml"),`lib,` sv`xml,.z.o)2:(`parseFile;2)

////////////
// PUBLIC //
////////////

///
// Parses the specified XML file by applying an XPath expression
// @param file symbol XML file
// @param xPath string XPath to be applied
.xml.parseFile:{[file;xPath]
  .xml.priv.parseFile[file;xPath]}

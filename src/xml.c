#include <k.h>

#include <libxml/xpathInternals.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 1000

#define MALLOC(x,y) x=malloc((y)->n*sizeof(char)+1),free_##x=1
#define FREE(x) if(free_##x)free(x)
#define KRR(...) char buffer[MAX_SIZE];sprintf(buffer,__VA_ARGS__); return krr(buffer)

K parseFile(K x, K y);
K execute_xpath_expression(const char *file, const xmlChar *xpath);
K build_xpath_nodes(xmlNodeSetPtr nodes);
K get_properties(xmlAttrPtr attribute);
K get_content(xmlNodePtr node);

K parseFile(K x, K y)
{
    char free_xpath = 0;
    int i;
    char *file;
    char *xpath;

    // Ensure x is of type symbol
    if (x->t == -KS) {
        file = x->s;
        if (file[0] == ':') {
            ++file;
        }
    } else {
        return krr("type");
    }

    // Ensure y is of type string
    if (y->t == KC) {
        MALLOC(xpath, y);
        for (i = 0; i < y->n; ++i) {
            xpath[i] = kC(y)[i];
        }
        xpath[i] = '\0';
    } else {
        return krr("type");
    }

    // Init libxml
    xmlInitParser();

    K result = execute_xpath_expression(file, (xmlChar*)xpath);

    // Shutdown libxml
    xmlCleanupParser();

    // Free any manually allocated memory
    FREE(xpath);

    return result;
}

K execute_xpath_expression(const char *filename, const xmlChar *xpath)
{
    xmlDocPtr doc;
    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;

    assert(filename);
    assert(xpath);

    doc = xmlParseFile(filename);
    if (!doc) {
        KRR("unable to parse file - %s", filename);
    }

    // Create xpath evaluation context
    xpathCtx = xmlXPathNewContext(doc);
    if(xpathCtx == NULL) {
        xmlFreeDoc(doc); 
        KRR("unable to create new XPath context");
    }

    // Evaluate xpath expression
    xpathObj = xmlXPathEvalExpression(xpath, xpathCtx);
    if(xpathObj == NULL) {
        xmlXPathFreeContext(xpathCtx); 
        xmlFreeDoc(doc);
        KRR("unable to evaluate Xpath expression - %s", xpath);
    }

    // Build results
    K result = build_xpath_nodes(xpathObj->nodesetval);

    // Cleanup
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx); 
    xmlFreeDoc(doc); 

    return result;
}

K build_xpath_nodes(xmlNodeSetPtr nodes)
{
    xmlNodePtr cur;
    int size;
    int i;

    // Create a mixed list
    size = (nodes) ? nodes->nodeNr : 0;
    K values = knk(3, ktn(KS, size), ktn(0, size), ktn(0, size));

    K keys = ktn(KS, 3);
    kS(keys)[0] = ss("name");
    kS(keys)[1] = ss("properties");
    kS(keys)[2] = ss("content");

    for (i = 0; i < size; ++i) {
        assert(nodes->nodeTab[i]);

        cur = nodes->nodeTab[i];

        kS(kK(values)[0])[i] = ss((S)cur->name);
        kK(kK(values)[1])[i] = knk(1, get_properties(cur->properties));
        kK(kK(values)[2])[i] = get_content(cur);
    }

    // Return table of data
    return xT(xD(keys, values));
}

K get_properties(xmlAttrPtr attribute)
{
    K keys = ktn(KS, 0);
    K values = ktn(KS, 0);

    while (attribute) {
        xmlChar *value = xmlNodeListGetString(attribute->doc, attribute->children, 1);

        js(&keys, ss((S)attribute->name));
        js(&values, ss((S)value));

        xmlFree(value);
        attribute = attribute->next;
    }

    return xD(keys, values);
}

K get_content(xmlNodePtr node)
{
    K result = ks("");

    // Count child nodes
    int i = 0;
    xmlNodePtr child = node->children;
    while (child) {
        ++i;
        child = child->next;
    }

    // Do not get content if we have more than 1 child
    if (i <= 1) {
        xmlChar *value = xmlNodeGetContent(node);
        if (value) {
            // Free empty string
            r0(result);

            result = kp((S)value);

            xmlFree(value);
        }
    }

    return result;
}

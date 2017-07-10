Webform Node Element is a module that allows you to include node content as markup in a webform. 

Basic Usage
===========

1. In a webform add a new 'Node' element. On the settings page for the node enter the nid of the node that you want to display, and the display mode that you wish to use (eg full, teaser, etc).

2. When viewing the webform the chosen node should be displayed using the chosen display mode.


Advanced Usage
==============

To dynamically set the node or display mode, subscribe to the 

    WebformNodeElementPreRender::PRERENDER

event, and call the 

    setNid
    setDisplayMode

methods. See the webform_node_element_example module for a working example..

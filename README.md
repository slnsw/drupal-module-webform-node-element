Webform Node Element is a module that allows you to include node content as markup in a webform. NB The content type that you wanto to display must have a display mode named 'webform_element'.

Basic Usage
===========

1. In a webform add a new 'Node' element. On the settings page for the node enter the nid of the node that you want to display

2. When viewing the webform the chosen node should be displayed using the webform_element display mode.


Advanced Usage
==============

@todo
- Changing the display mode
- Dynamically changing the node id
Overview: This can be done by subscribing to the webform_element_node.pre_render event

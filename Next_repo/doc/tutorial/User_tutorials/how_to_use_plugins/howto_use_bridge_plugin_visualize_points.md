How to visualize points using Bridge Plugins {#howto_use_bridge_plugin_visualize_points}
=====

[TOC]

---

# Overview {#howto_use_bridge_plugin_visualize_points_overview}

In the tutorial we will learn how to visualize point clouds in the NEXT-GUI.

# Prerequisites
* A dataset containing points
* Next Framework
* Next-GUI
* Regular expression patterns

### Dataset containing points
A dataset containing points that you aim to visualize, for example a recording.
The backend system is compatible with both Cartesian coordinates (x, y, z) and spherical coordinates (radius, theta, phi).

### Next Framework
* Next-Player
* Next-ControlBridge
* Next-DataBridge

### Next-GUI
The GUI widgets are driven by data and rely significantly on the JSON data received from the backend.
 They exclusively support the Cartesian coordinate system, spherical coordinates are automatically converted to Cartesian coordinates by the backend.
> 🔗[GUI documentation about point clouds](https://github-am.geo.conti.de/pages/ADAS/Next-GUI/#/./interface/3d_view?id=point-clouds)

### Regular expression patterns
Regular expression patterns for identifying URLs referencing signal data related to point clouds.

* Create an regular expression pattern for your signal
![Regular expression pattern](pointcloud_regex.png)
* Add your regular expression to config
![Global Config Regular Expressions](pointcloud_global_config.png)
![Add Regular Expressions](pointcloud_add_regex.png)
* You could also add your search request in code
![Init Search Request](pointcloud_init_search_requests.png)

# Steps
* Start Next backend and Next GUI, Load recording
> 🔗 @ref howto_use_gui

* Add 3D View Widget
![Add widget](pointcloud_add_widget.png)

* Open data stream configuration
<!-- align image to the left -->
<div style="text-align: left; margin-left: 45px;">
![Add datastream config](pointcloud_datastream_config.png)
</div>

* Add Point Clouds view3D_PointCloud
![Select plugin](pointcloud_select_plugin.png)
* Play recording and visualize cloud points
![Visualize cloud points](pointcloud_visualize.png)


Plugin Regular Expression Searcher {#plugins_system_regex_searcher}
=====

[TOC]

## Overview {#plugins_system_regex_searcher_overview}
Searching signals within the signal structure and matching them can be a hassle. The RegexSearcher is a helper class that aids all plugins in pattern matching URLs efficiently and in a generic way, preventing code duplication across plugins.


## Usage instructions {#plugins_system_regex_searcher_usage}
1) In your plugin, identify a keyword to locate all relevant URLs. 
 - For instance, 'object' keyword is often used in Traffic participant object lists.
2) Define custom search requests for each URL required by plugin
 - A SearchRequest includes:
   - **search_expression**: a vector of strings with regular expressions
   - **data_type_name**: string
   - **isOptional**: a boolean to indicate if the SearchRequest is optional
3) Plugins should instantiate PluginRegexSearcher and initialize it with these custom search requests using the ```InitializeSearchRequests()``` method
   - if there are updates to the regular expressions in the global configuration,  reinitialize the searcher.

4) When using ```GetSearchResults()```, plugins must supply a list of relevant URLs (identified in step 1). The searcher ensures there are no duplicated data (like arrays) in this list and narrows it down to just individual array entries (such as ObjectList[0]).

 - Additionally, you can target specific URL groups using the ``GetSearchResultForUrl()`` method. This will filter the list to only include URLs from the specified group.

5) ``GetSearchResults()`` returns a vector of SearchResults
 - A SearchResult contains:
   - root_url: a string that indicates the main group URL.
   - data_type_to_result: an unordered map of <string, Result> pairs. Each pair maps a data_type_name to its corresponding matched result which contains result_url and regex_pattern.
   - For example, a SearchResult could be:
   ```cpp 
    // root url
   {"Device.View.SomeTrafficParticipantList", 
     // data     result               regex_pattern
     {"pos_x", {".kinematic.fDistX", "(\.kinematic\.fdistx)"},
     {"pos_y", {".kinematic.fDistY", "(\.kinematic\.fdisty)"}
   }
   ```

## Constructing search requests for a plugin

1) In scenarios requiring a selection from different types of data, such as different coordinate systems in a PointCloud plugin, search requests can be marked as **optional**
   - The **optional** setting will let the searcher know that a search result can be returned with or without a match pattern for the particular request.
   - If the **optional** setting is set to ``false`` for a give search request and the search  fails to find a match result, it will return an empty vector of search results. 
   - In situations involving multiple optional search requests, the plugin must effectively filter out non-relevant results (for instance, discarding results lacking either cartesian or spherical coordinate data).
   - Example of search requests:
```cpp
  bool optional = true;
  keywords_ = {"Output", "ClusterListArrays"};

  std::vector<std::tuple<std::string, std::vector<std::string>, bool>> requests = {
      {"point_", {R"(((\.ussPointList\.)?([^.]+))\[\d+\])"}, optional},
      {"xpos", {R"((\.[x]?position.*(?:_m|[x]|_m_f32)$))", R"((x_m$))"}, optional},
      {"ypos", {R"((\.[y]?position.*(?:_m|[y]|_m_f32)$))", R"((y_m$))"}, optional},
      {"zpos", {R"((\.[z]?position.*(?:_m|[z]|_m_f32)$))", R"((z_m$))"}, optional},
      {"radius_", {R"((\.a_RangeRad)\[\d\]$)"}, optional},
      {"phi_", {R"((\.a_azang)\[\d\]$)"}, optional},
      {"theta_", {R"((\.a_elevang)\[\d\]$)"}, optional},
      {"num_points", {R"(((\.ussPointList\.)?num.*point.*))"}, optional},
      {"arrays", {R"((\.clusterlistarrays)\.)"}, optional}
  };

  for (const auto &[key, patterns, isOptional] : requests) {
   search_requests_.emplace_back(SearchRequest{key, patterns, isOptional});
  }
  
  plugin_regex_searcher_->InitializeSearchRequests(search_requests_);
```


2) The search result's **root_url** will always be the group URL . Construct the regular expressions so they match the necessary URLs starting at the topic level. 
   - For instance, consider the following URL: ```ARS5xxDevice.EM_Data.ObjectList.ObjListInfo.NumberOfObjects```
   - The **result_url** for a request called ```number_of_objects``` should return ```.ObjListInfo.NumberOfObjects``` as the result_url.

3) By default, the searcher does not handle data at subgroup levels, but this can be taken care of from the plugins by adjusting regular expressions as mentioned above.



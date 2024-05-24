How to load a PackageProcessor {#howto_load_package_processor}
=====

[TOC]

---

* @ref howto_package_processor_overview
* @ref howto_setup_package_processor
* @ref howto_package_processor_example
* @ref howto_load_package_processor
* @ref howto_debug_package_processor

---

# Summary

Loading of a PackageProcessor within the \named{NEXT-Player}\glos{player} needs to be explicitly configured.

> ❗ relative paths are read using the current working directory of the player ❗

Use the configuration system for setting the path to your processor. Example:

```
{
  "next_player": {
    "udex": {
      "package_processor": {
        "search_paths": [
          ".\/." #<<<<<<<<<<<<<<< These are the searched paths. Multiple entries are possible.
        ],
      }
    }
  }
}
```

After a successful loading your requested headers should appear via

```
{
  "next_player": {
    "udex": {
      "package_processor": {
        "search_paths": [
          ".\/."
        ],
        "instances": {
          "dummy_processor.dll0": {
            "cycle_id": "100",
            "instance_number": "0",
            "source_id": "9353",
            "v_address": "2147483648"
          },
          "test_processor.dll0": {
            "cycle_id": "100",
            "instance_number": "0",
            "source_id": "9353",
            "v_address": "2147483648"
          }
        }
      }
    }
  }
}
```





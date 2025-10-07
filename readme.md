# CARLCRAFT!!!!

I might change the name later, but im having fun... i belive.



## CMakeUserPresets.json for vcpkg

This is my CMakeUserPresets.json that i use on windows
```
{
  "version": 3,
  "configurePresets": [
    {
      "name": "default",
      "inherits": "vcpkg",
      "environment": {
        "VCPKG_ROOT": "C:/vcpkg"
      }
    }
  ]
}
```

If you add this file it should work pretty automatically after building the project if youre using visual studio, and perhaps without it.
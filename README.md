# 4d-plugin-lha
Decompress LZH

4D implementation of [lhasa](https://github.com/fragglet/lhasa)

### Platform

| carbon | cocoa | win32 | win64 |
|:------:|:-----:|:---------:|:---------:|
|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|

### Version

<img src="https://cloud.githubusercontent.com/assets/1725068/18940649/21945000-8645-11e6-86ed-4a0f800e5a73.png" width="32" height="32" /> <img src="https://cloud.githubusercontent.com/assets/1725068/18940648/2192ddba-8645-11e6-864d-6d5692d55717.png" width="32" height="32" /> <img src="https://user-images.githubusercontent.com/1725068/41266195-ddf767b2-6e30-11e8-9d6b-2adf6a9f57a5.png" width="32" height="32" />

### Releases

[2.0](https://github.com/miyako/4d-plugin-lha/releases/tag/2.0)

![preemption xx](https://user-images.githubusercontent.com/1725068/41327179-4e839948-6efd-11e8-982b-a670d511e04f.png)

``$3`` has been changed from ``ARRAY TEXT`` to ``TEXT`` in this version for ``threadSafe``

[1.0](https://github.com/miyako/4d-plugin-lha/releases/tag/1.0)

### Remarks

Unicode paths on Windows is not supported (ANSI API is used for ``fopen``, ``CreateDirectoryA``, etc.)

## Syntax

```
LHA (src;dst{;paths})
```

Parameter|Type|Description
------------|------------|----
src|TEXT|
dst|TEXT|
paths|TEXT|on input, ``glob`` style filters; on output, extracted paths

All paths in system style (HFS on macOS).

Source (i.e. not target) path is returned for symbolic links.

### Examples (v1)

```
$src:=Get 4D folder(Current resources folder)+"sample-lzh.lzh"
$dst:=System folder(Desktop)

ARRAY TEXT($paths;1)

$paths{1}:="*.jpg"  //extract jpg files

LHA ($src;$dst;$paths)
```

### Error Codes

```c
typedef enum
{
	lha_error_none = 0,
	lha_error_extract_archive = 1,
	lha_error_fopen = 2,
	lha_error_input_stream_from_file = 3,
	lha_error_reader_new = 4
}lha_error_t;
```

# 4d-plugin-lha
Decompress LZH

4D implementation of [lhasa](https://github.com/fragglet/lhasa)

## Syntax (draft)

```
LHA (src;dst{;paths})
```

Parameter|Type|Description
------------|------------|----
src|TEXT|
dst|TEXT|
paths|ARRAY TEXT|on input, ``glob`` style filters. on output, extract paths

### Examples

```
$src:=Get 4D folder(Current resources folder)+"sample-lzh.lzh"
$dst:=System folder(Desktop)

ARRAY TEXT($paths;1)

$paths{1}:="*.jpg"  //extract jpg files

LHA ($src;$dst;$paths)
```

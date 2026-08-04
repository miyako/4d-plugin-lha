//%attributes = {"preemptive":"capable"}
$src:=Get 4D folder:C485(Current resources folder:K5:16)+"sample-lzh.lzh"
$dst:=System folder:C487(Desktop:K41:16)

$json:=""

$e:=LHA($src; $dst; $json)
ARRAY TEXT:C222($paths; 0)
JSON PARSE ARRAY:C1219($json; $paths)
ALERT:C41(String:C10(Size of array:C274($paths)))

C_COLLECTION:C1488($col)
$col:=New collection:C1472("*.jpeg")

$json:=JSON Stringify:C1217($col)

LHA($src; $dst; $json)

ARRAY TEXT:C222($paths; 0)
JSON PARSE ARRAY:C1219($json; $paths)

ALERT:C41(String:C10(Size of array:C274($paths)))
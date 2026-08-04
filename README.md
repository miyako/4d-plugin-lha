# 4d-plugin-lha

4D implementation of [lhasa](https://github.com/fragglet/lhasa)

The `lha` plugin extracts LZH/LHA archives (`.lzh`/`.lha` files) to a destination folder from 4D, optionally filtering which entries get extracted by wildcard pattern, and reports back the full path of every file it actually wrote. It exposes a single command, `LHA`, and returns a `Longint` error code rather than raising a 4D error.

| Command | Returns | Purpose |
|---|---|---|
| [`LHA`](#lha) | Longint | Extract an archive to a folder, optionally filtered, and get back the list of extracted file paths. |

**Platforms:** macOS and Windows.

---

## Requirements & platform notes

- All three parameters are mandatory and positional — there's no optional form. To extract every entry with no filtering, pass an empty string (`""`) as the third parameter, as shown in the plugin's own sample method.
- The third parameter is used for both input and output: pass in a filter as JSON text, and on return the same variable is overwritten with a JSON array of the full paths of every file that was extracted.
- The command never raises a 4D runtime error or interrupts the calling method on failure — always check the returned `Longint`. See [Error handling & troubleshooting](#error-handling--troubleshooting) for what each value means.
- **On Windows**, the source archive path is converted through a fixed-size internal buffer capped at `MAX_PATH` (260 wide characters). A source path longer than that fails the same way as a missing/unreadable file (return code `2`) — there's no separate code for "path too long."
- The manifest declares this command `threadSafe`, and the plugin's own sample method is tagged `preemptive: capable`, so it's safe to call from a preemptive process.

---

## LHA

### Syntax

```4d
LHA ( archive ; destination ; filters ) → Longint
```

| Parameter | Type | Description |
|---|---|---|
| `archive` | Text | Full path to the source `.lzh`/`.lha` archive. |
| `destination` | Text | Folder path the archive's contents are extracted into. |
| `filters` | Text | **In:** JSON text — either an empty string (`""`) to extract every entry, or a JSON array of wildcard filter patterns (e.g. `["*.jpg","*.png"]`) to extract only matching entries. **Out:** overwritten on return with a JSON array of the full paths of every file actually extracted. |
| Result | Longint | Error code. `0` = success. See [Error handling & troubleshooting](#error-handling--troubleshooting) for the full list. |

### Description

Pass the archive path, a destination folder, and a filter. The filter parameter does double duty: whatever you pass in is read as JSON before extraction starts, and the *same variable* is overwritten with the result — a JSON array of every path the plugin wrote — once extraction finishes. That's why the sample method below reuses `$json` as both an input and an output.

An empty string for `filters` means "extract everything." A JSON array of strings is treated as a set of wildcard patterns matched against archive entries — exactly what pattern syntax is supported (e.g. whether `*` crosses directory separators, whether matching is case-sensitive) is determined by the plugin's underlying filter library rather than anything in the 4D-facing layer, so if a specific pattern doesn't match what you expect, treat that as a filter-library detail to test directly rather than something this reference can fully specify.

If any element of the filters array isn't a plain string (for example a number, `Null`, or a nested object/array), that element is simply skipped — it does not abort the call or affect the other filters.

Extraction runs synchronously on the calling process, yielding periodically to the host so the interface stays responsive during a long extraction; it does not spawn a separate 4D process.

### Example

From the plugin's own test method (`Method1.4dm`):

```4d
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
```

The first call extracts everything (empty filter) and reports how many files were extracted. The second call re-extracts the same archive, this time filtered to only `*.jpeg` entries.

A variation filtering on more than one pattern, and checking the result code before trusting the output — built from the same commands as the sample above, plus 4D's core `If`/`Else` control flow:

```4d
$src:=Get 4D folder:C485(Current resources folder:K5:16)+"sample-lzh.lzh"
$dst:=System folder:C487(Desktop:K41:16)

C_COLLECTION:C1488($col)
$col:=New collection:C1472("*.jpg"; "*.png")
$json:=JSON Stringify:C1217($col)

$e:=LHA($src; $dst; $json)

If ($e=0)
	ARRAY TEXT:C222($paths; 0)
	JSON PARSE ARRAY:C1219($json; $paths)
	ALERT:C41(String:C10(Size of array:C274($paths))+" file(s) extracted")
Else
	ALERT:C41("Extraction failed, error code: "+String:C10($e))
End if 
```

---

## Error handling & troubleshooting

- **Always check the returned `Longint`, not just the output array.** The command never raises a 4D error or shows an alert on failure — a failed call still returns and still overwrites the third parameter (typically with an empty array), so the only reliable signal is the return code.
- **Code `2` — archive couldn't be opened.** Covers a missing file, a permissions problem, and (Windows only) a source path longer than the internal 260-wide-character limit — these all produce the identical code `2`, with nothing in the return value to tell them apart.
- **Code `3` — the archive's data stream couldn't be read**, once the file itself opened successfully.
- **Code `4` — the archive reader couldn't be created** from an otherwise-readable stream, typically indicating the file isn't a valid LZH/LHA archive.
- **Code `1` — extraction itself failed partway through.** Some or all entries may not have been written; check the paths returned in `filters` to see what did complete.
- **Code `5` — an internal exception was caught.** This code only exists in the reviewed/fixed build of this plugin; earlier builds did not guarantee a return in this situation. Treat it as "something unexpected happened before extraction could run to completion" rather than a specific diagnosis.
- **Passing `Null` or omitting the filter parameter isn't the same as "no filter."** Use an explicit empty string (`""`) to mean "extract everything" — the parameter is always required and always read as text.

---

## Quick reference

```4d
// Extract everything
$json:=""
$e:=LHA($src; $dst; $json)

// Extract only matching entries
$json:=JSON Stringify(New collection("*.jpg"; "*.png"))
$e:=LHA($src; $dst; $json)

// Read back what was extracted
If ($e=0)
	ARRAY TEXT($paths; 0)
	JSON PARSE ARRAY($json; $paths)
End if
```

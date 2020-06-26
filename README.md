# gsufsort: 

This software is a fast, portable, and lightweight tool for constructing the **suffix array** and related data structures for **string collections**. 

The software runs in internal memory (data structures are written to disk). 

Given an string collection, **_gsufsort_** can compute the:

- [x] Suffix array (SA)
- [x] Inverse suffix array (ISA)
- [x] LCP-array (LCP)
- [x] k-truncated LCP-array (k-LCP)
- [x] Document array (DA)
- [x] Burrows-Wheeler transform (BWT)
- [x] Inverse BWT
- [x] Generalized suffix array (GSA)
- [x] Generalized enhanced suffix array (GESA)

## install

```sh
git clone https://github.com/felipelouza/gsufsort.git
cd gsufsort
make 
```

## run

Given a string collection in a single file INPUT.

```sh
./gsufsort INPUT [options]
```

### Available options

```sh
--build	              (default)
--load                load from disk INPUT[.sa][.da][.lcp][.gsa][.str]
--sa    [w]           compute SA  (default) using w (def 4) bytes (INPUT.w.sa)
--isa   [w]           compute ISA (INPUT.w.isa)
--lcp   [w]           compute LCP (INPUT.w.lcp)
--da    [w]           compute DA  (INPUT.w.da)
--gsa   [w1][w2]      compute GSA=(text, suff) using pairs of (w1, w2) bytes (INPUT.w1.w2.gsa)
--gesa  [w1][w2][w3]  compute GESA=(GSA, LCP, BWT) (INPUT.w1.w2.w3.1.gesa)
--light               run lightweight algorithm to compute DA (also GSA and GESA)
--bwt                 compute BWT using 1 byte per symbol (INPUT.bwt)
--ibwt                invert the BWT, given INPUT[.bwt]
--qs                  output QS sequences (only for fastq) permuted according to the BWT (INPUT.qs)
--str                 output T^{cat} in ASCII format (INPUT.1.str)
--docs    d           number of strings to be handled (def all)
--print   [p]         print arrays (stdout) A[1,min(p,N)]
--lcp_max             output maximum LCP-value
--lcp_max_text        output maximum LCP-value (text)
--lcp_avg             output average LCP-value
--trlcp   k           output k-truncated LCP array (INPUT.w.lcp)
--lower               converts input symbols to lowercase
--upper               converts input symbols to uppercase 
--time                output time (in seconds)
--txt                 handle input (INPUT) as raw file (one string per line)
--fasta               handle input (INPUT) as FASTA 
--fastq               handle input (INPUT) as FASTQ
--dir                 handle multiple files in directory (INPUT) as input
--output  outfile     rename output file
--verbose             verbose output
--help                this help message
```

### Input files 

- **Supported extensions**: _.txt_, _.fasta_ (or _.fa_, _.fna._) and _.fastq_ (or _.fq_).

- Strings are separated per '\0' (new line) in _.txt_ files.

- **_gsufsort_** supports **ASCII alphabet**, so that values _0_ and _1_ are reserved.

- **gzipped input data** (extension _.gz_) are supported uzing [zlib](https://github.com/felipelouza/gsufsort/tree/master/external/zlib) and [kseq](https://github.com/felipelouza/gsufsort/tree/master/external/kseq) libraries. Please, build with the option ``make GZ=1``.

- **Multiple files** in a given directory (_INPUT_) are supported with option ``--dir``, see [below](https://github.com/felipelouza/gsufsort#multiple-files).

- For inputs **larger than 2GB**, use **_gsufsort-64_**

### Output files 

- Output data structures (for example ``INPUT.4.sa``) are written in binary format, in which each integer takes ``w`` bytes (def. ``w`` is 4).

- Output files are written (**by default**) in the current directory, in which **gsufsort** is executed, see [below](https://github.com/felipelouza/gsufsort#output).

- Option ``--output DIR/`` renames the target directory to ``DIR/``, while ``--output DIR/FILENAME`` renames output file names to ``DIR/FILENAME``.

## quick test

To run a test with ``docs=3`` strings from ``dataset/example.txt``, type:

```sh
./gsufsort dataset/example.txt --docs 3 --sa --bwt
```

```sh
## gsufsort ##
## store_to_disk ##
example.txt.4.sa	76 bytes (n = 19)
example.txt.bwt	19 bytes (n = 19)
```

To see the result (option ``--print``) stored in disk ``INPUT.4.sa`` and ``INPUT.bwt``, use ``--load`` option:

```sh
./gsufsort dataset/example.txt --sa --bwt --load --print
```

```sh
## load_from_disk ##
example.txt.4.sa	76 bytes (n = 19)
example.txt.bwt	19 bytes (n = 19)
i	SA	BWT	suffixes
0	18	$	#
1	6	a	$
2	12	a	$
3	17	n	$
4	5	n	a$
5	11	b	a$
6	9	n	aba$
7	15	n	an$
8	3	n	ana$
9	7	$	anaba$
10	13	$	anan$
11	1	b	anana$
12	10	a	ba$
13	0	#	banana$
14	16	a	n$
15	4	a	na$
16	8	a	naba$
17	14	a	nan$
18	2	a	nana$
```

### output

The **suffix array** output (``INPUT.4.sa``) is written in binary format, each integer takes ``w`` bytes (default ``w`` is 4).

```sh
ls -la example.txt.4.sa
-rw-rw-r--. 1 louza louza 76 Apr 23 08:25 example.txt.4.sa
```

The **BWT** output (``INPUT.bwt``) is written in ASCII format:

```sh
less +1 dataset/example.txt.bwt
^Aaannbnnn^A^Aba^@aaaaa
```

We can **invert the BWT** with option ``--ibwt``:

```sh
./gsufsort dataset/example.txt.bwt --ibwt
```

The result is stored in ``INPUT.ibwt``, which can be compared with the orignal file:

```sh
diff -s dataset/example.txt.ibwt dataset/example.txt
Files dataset/example.txt.ibwt and dataset/example.txt are identical
```

#### Notes:

For **fasta** files, one can compare ``INPUT.ibwt`` using the commands:

```sh
awk '!/^>/ { printf "%s", $0; n = "\n" } /^>/ { print n $0; n = "" } END { printf "%s", n }' INPUT | sed '/^>/d' - | diff INPUT.ibwt -
```

While for **fastq** files, one can use the commands:

```sh
sed -n 2~4p INPUT | diff INPUT.ibwt -
```


## remarks

* The linear time algorithm [gsaca-k](https://github.com/felipelouza/gsa-is) is at the core of **_gsufsort_**. In particular, it is used to compute SA, LCP and DA.

* For inputs **larger than 2GB**, **_gsufsort-64_** uses **21N bytes** to compute SA, LCP and DA (GESA).

#### working memory (in bytes)

| version   | 32 bits | 64 bits |
|-----------|:-------:|:-------:|
| SA        |    5N   |    9N   |
| SA+LCP    |    9N   |   17N   |
| SA+LCP+DA |   13N   |   21N   |
| SA+DA     |    9N   |   13N   |
| GSA       |    9N   |   13N   |
| GESA      |   13N   |   21N   |

#### _lightweight_ version

* There is a _lightweight_ version of **_gsufsort_** (option ``--light``) to compute DA using a bitvector during the output to disk, which saves **4N bytes**. 

* For inputs **larger than 2GB**, the _lightweight_ version uses **17N bytes** to compute SA, LCP and DA (GESA).


## additional features 

### _Multiple files_

* **_gsufsort_** supports multiple files in directory INPUT (command ``--dir``):

For example, ``dataset/input.txt`` were splitted into 5 files in ``dataset/input/``:

```sh
ls dataset/input/
input-1.txt  input-2.txt  input-3.txt  input-4.txt  input-5.txt
```

One can index all files in ``dataset/input/`` with options ``--dir`` and ``--sa``:

```sh
./gsufsort dataset/input/ --dir --sa
```

In the case no argument is given for ``--output``, the default output filename is ``./all``.

```sh
## gsufsort ##
## store_to_disk ##
all.4.sa 	20198960 bytes (n = 5049740)
```

Compare the output ``all.4.sa`` with ``dataset/input.txt.4.sa``:

```sh
diff all.4.sa dataset/input.txt.4.sa -s
Files all.4.sa and dataset/input.txt.4.sa are identical
```

We can also output the concatenations of all files in ``all.str`` (option ``--str``):

```sh
./gsufsort dataset/input/ --dir --sa
```

```sh
## gsufsort ##
## store_to_disk ##
all.1.str 5049740  bytes (n = 5049740)
```

When we compare ``all.str`` with ``dataset/input.txt``, there is an extra byte in ``all.str`` corresponding to the terminator \# added to the concatenated string:

```sh
ls all.1.str dataset/input.txt -la
-rw-rw-r--. 1 louza louza 5049740 Jun 26 09:54 all.1.str
-rw-rw-r--. 1 louza louza 5049739 Jun 25 10:19 dataset/input.txt
```

```sh
diff all.1.str dataset/input.txt -s
10001d10000
<
```

* **Awarning**: in the case one use options ``--txt``, ``--fasta`` or ``-fastq`` together with ``--dir``, be carefull that INPUT contains only valid files, otherwise the program may crash.

### _quality score (QS) sequences_

* **_gsufsort_** can also output (command ``--qs``) the _Quality Scores_ (QS) permuted according to the BWT symbols:

* This option is valid only for ``.fastq`` or ``.fq`` files.

For example, given the first DNA read in ``dataset/reads.fastq``:

```sh
head -4 dataset/reads.fastq 
@HWI-ST928:79:C0GNWACXX:6:1101:1184:2104 1:N:0:TAAGGCGATATCCTCT
AGTTAGGACTATTCGAACATTATGTCACAAACGTGATGTCACAAAGCCGAATTGTCTGGAGTTAAGACTATACGAACATTATGAAACAAACGTGATGTCAC
+
@C@FDEDDHHGHHJIIGGHJJIJGIJIHGIIFGEFIIJJJGHIGGF@DHEHIIIIJIIGGIIIGE@CEEHHEE@B?AAECDDCDDCCCBB<=<?<?CCC>A
```

Then, run:

```sh
./gsufsort dataset/reads.fastq --docs 1 --bwt --qs
```

```sh
## gsufsort ##
## store_to_disk ##
dataset/reads.fastq.bwt	103 bytes (n = 103)
dataset/reads.fastq.bwt.qs	103 bytes (n = 103)
```

The _QS_ permuted sequence is written at ``dataset/reads.fastq.bwt.qs``:

```sh
tail dataset/reads.fastq.bwt.qs 
ACCHHD@ICGIIHCDJJBIHBI@DGGFGEC?JFAGHE>CIGCIJ?GFEH@BICDIDEJDEEI<EGDI?JII<FG@IH@EEJHCGJHID=GJ<IIIICAHGH
```

**_gsufsort_** can invert the __QS permuted sequence__ together with the BWT (options ``--ibwt --qs``).

```sh
./gsufsort --ibwt --qs dataset/reads.fastq.bwt
```

See the resulting file:

```sh
less +1 dataset/reads.fastq.iqs
@C@FDEDDHHGHHJIIGGHJJIJGIJIHGIIFGEFIIJJJGHIGGF@DHEHIIIIJIIGGIIIGE@CEEHHEE@B?AAECDDCDDCCCBB<=<?<?CCC>A
```

Compare the output with the original file:

```sh
head -4 dataset/reads.fastq | sed -n 4~4p - | diff -s dataset/reads.fastq.iqs -
Files dataset/reads.fastq.iqs and - are identical
```


## authors

* [Felipe Louza](https://github.com/felipelouza)
* [Guilherme Telles](https://github.com/gptelles)
* [Simon Gog](https://github.com/simongog)
* [Nicola Prezza](https://github.com/nicolaprezza)
* [Giovanna Rosone](https://github.com/giovannarosone/)

## thanks

Thanks to [Antonis Maronikolakis](https://github.com/antmarakis) by helpful suggestions.


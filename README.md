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

- **Multiple files** in a given directory (_INPUT_) are supported with option ``--dir``, see [Wiki](https://github.com/felipelouza/gsufsort/wiki/Multiple-Files).

- For inputs **larger than 2GB**, use **_gsufsort-64_**

### Output files 

- Output data structures (for example ``INPUT.4.sa``) are written in binary format, in which each integer takes ``w`` bytes (def. ``w`` is 4).

- Output files are written (**by default**) in the current directory, in which **gsufsort** is executed, see [below](https://github.com/felipelouza/gsufsort#output).

- Option ``--output DIR/`` renames the target directory to ``DIR/``, while ``--output DIR/FILENAME`` renames output file names to ``DIR/FILENAME``.

## quick test

To run a test with all strings from ``dataset/example.txt``, type:

```sh
./gsufsort dataset/example.txt --sa --bwt
```

```sh
## gsufsort ##
## store_to_disk ##
example.txt.4.sa	76 bytes (n = 19)
example.txt.bwt	19 bytes (n = 19)
```

To see the result (option ``--print``) stored in disk ``INPUT.4.sa`` and ``INPUT.bwt``, use ``--load`` option:

```sh
./gsufsort example.txt --sa --bwt --load --print
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
less +1 example.txt.bwt
^Aaannbnnn^A^Aba^@aaaaa
```

We can **invert the BWT** with option ``--ibwt``:

```sh
./gsufsort example.txt.bwt --ibwt
```

The result is stored in ``INPUT.ibwt``, which can be compared with the orignal file:

```sh
diff -s example.txt.ibwt dataset/example.txt
Files example.txt.ibwt and dataset/example.txt are identical
```

## wiki

See more details and additional features in [Wiki](https://github.com/felipelouza/gsufsort/wiki).

## authors

* [Felipe Louza](https://github.com/felipelouza)
* [Guilherme Telles](https://github.com/gptelles)
* [Simon Gog](https://github.com/simongog)
* [Nicola Prezza](https://github.com/nicolaprezza)
* [Giovanna Rosone](https://github.com/giovannarosone/)

## thanks

Thanks to [Antonis Maronikolakis](https://github.com/antmarakis) by helpful suggestions.


# gsufsort: 

This software is an implementation of **_gsufsort_**, a fast, portable, and lightweight tool for constructing the **suffix array** and related data structures for **string collections**. 

The software runs in internal memory (the computed data structures are written to disk). 

Given an string collection, **_gsufsort_** can compute the:

- [x] Suffix array (SA)
- [x] LCP-array (LCP)
- [x] k-truncated LCP-array (k-LCP)
- [x] Document array (DA)
- [x] Burrows-Wheeler transform (BWT)
- [x] Generalized suffix array (GSA)
- [x] Generalized enhanced suffix array (GESA)

## install

```sh
git clone https://github.com/felipelouza/gsufsort.git
cd gsufsort
make 
```

## run

Given a string collection in a single file FILENAME.

```sh
./gsufsort FILENAME [options]
```

#### Available options

```sh
--build	              (default)
--load                load from disk FILENAME[.sa][.da][.lcp][.gsa][.bin]
--sa    [w]           computes SA  (default) using w (def 4) bytes (FILENAME.w.sa)
--lcp   [w]           computes LCP (FILENAME.w.lcp)
--da    [w]           computes DA  (FILENAME.w.da)
--gsa   [w1][w2]      computes GSA=(text, suff) using pairs of (w1, w2) bytes (FILENAME.w1.w2.gsa)
--light               runs lightweight algorithm to compute DA (also GSA)
--gesa  [w1][w2][w3]  computes GESA=(GSA, LCP, BWT) (FILENAME.w1.w2.w3.1.gesa)
--bwt                 computes BWT using 1 byte (FILENAME.1.bwt)
--bin                 computes T^{cat} (FILENAME.1.bin)
--docs    d           number of strings (def all FILENAME)
--print   [p]         print arrays (stdout) A[1,min(p,N)]
--lcp_max             outputs maximum LCP-value
--lcp_max_text        outputs maximum LCP-value (text)
--lcp_avg             outputs average LCP-value
--trlcp   k           outputs k-truncated LCP array (FILENAME.w.lcp)
--time                outputs time (in seconds)
--output  outfile     renames output file
--verbose             verbose output
--help                this help message
```

#### Command

```sh
./gsufsort FILENAME [--sa [w]] [--lcp [w]] [--da [w]] [--ligth] [--gsa [w1] [w2]] [--gesa [w1] [w2] [w3]] [--bwt] [--bin] [--docs d] [--print [p]] [--lcp_max] [--lcp_max_text] [--lcp_avg] [--trlcp [k]] [--output out]
```

#### Input files 

- **Supported extensions**: _.txt_, _.fasta_ (or _.fa_) and _.fastq_ (or _.fq_).

- **gzipped input data** (extension _.gz_) are also supported.

- Strings are separated per '\0' (new line) in _.txt_ files.

- _gsufsort_ supports **ASCII alphabet**, so that values _0_ and _1_ reserved.

- For inputs **larger than 2GB**, use _gsufsort-64_


## quick test

To run a test with _docs=3_ strings from _dataset/input.txt_, type:

```sh
./gsufsort dataset/input.txt --docs 3 --sa --bwt
```

```sh
## gsufsort ##
## store_to_disk ##
dataset/input.txt.4.sa	72 bytes (n = 18)
dataset/input.txt.1.bwt	18 bytes (n = 18)
malloc_count ### exiting, total: 32,859, peak: 21,420, current: 1,024
```

To see the result (option ``--print``) stored in disk ``FILENAME.4.sa`` and ``FILENAME.1.bwt``, use **--load** option:

```sh
./gsufsort dataset/input.txt --sa --bwt --load --print
```

```sh
## load_from_disk ##
dataset/input.txt.4.sa	72 bytes (n = 18)
dataset/input.txt.1.bwt	18 bytes (n = 18)
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
malloc_count ### exiting, total: 10,438, peak: 5,790, current: 1,024
```

### output files

In particular, the BWT output (``FILENAME.1.bwt``) is written in ASCII format, which can be opened in terminal:

```sh
vim dataset/input.txt.1.bwt
```

```sh
aannbnnn��ba\0aaaaa
```

Suffix array output (``FILENAME.4.sa``) is written in binary format, each integer takes ``w`` bytes (default ``w`` is 4).

```sh
ls -la dataset/input.txt.4.sa
```

```sh
-rw-rw-r--. 1 louza louza 72 Apr 23 08:25 dataset/input.txt.4.sa
```

### gzipped input files
_gsufsort_ also supports gzipped input files uzing [zlib](https://github.com/felipelouza/gsufsort/tree/master/external/zlib) and [kseq](https://github.com/felipelouza/gsufsort/tree/master/external/kseq) libraries:

```sh
make clean
make GZ=1
```

Then, run:

```sh
./gsufsort gz/input.txt.gz --docs 3 --sa --bwt
```

```sh
## gsufsort ##
## store_to_disk ##
gz/input.txt.gz.4.sa	72 bytes (n = 18)
gz/input.txt.gz.1.bwt	18 bytes (n = 18)
malloc_count ### exiting, total: 10,578,270, peak: 10,566,937, current: 1,024
```


## remarks

* The linear time algorithm [gsaca-k](https://github.com/felipelouza/gsa-is) is at the core of _gsufsort_. In particular, it is used to compute SA, LCP and DA.

* For inputs **larger than 2GB**, _gsufsort-64_ uses **21N bytes** to compute SA, LCP and DA (GESA).

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

* There is a _lightweight_ version of _gsufsort_ (option ``--light``) to compute DA using a bitvector during the output to disk, which saves **4N bytes**. 

* For inputs **larger than 2GB**, the _lightweight_ version uses **17N bytes** to compute SA, LCP and DA (GESA).


## authors

* [Felipe Louza](https://github.com/felipelouza)
* [Guilherme Telles](https://github.com/gptelles)
* [Simon Gog](https://github.com/simongog)
* [Nicola Prezza](https://github.com/nicolaprezza)
* [Giovanna Rosone](https://github.com/giovannarosone/)

## thanks

Thanks to [Antonis Maronikolakis](https://github.com/antmarakis) by helpful suggestions.

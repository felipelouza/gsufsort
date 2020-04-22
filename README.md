# gsufsort: 

This software is a fast, portable, and lightweight tool for constructing the **suffix array** (SA) and related data  structures for **string collections**, such as the **LCP array**, the **document array** (DA), the **generalized suffix array** (GSA) and the **Burrows-Wheeler transform** (BWT). 

The software runs in linear time and works in internal memory (the computed data strutures are written to disk). 

- Supported extensions: _.txt_, _.fasta_ and _.fastq_.

- **gzipped input data** (extension _.gz_) are also supported.

- For inputs **larger than 2GB**, use _gsufsort-64_

## install

```sh
git clone https://bitbucket.org/louza/gsufsort/ 
cd gsufsort
make 
```

## run

Given a string collection in a single file FILE.

```sh
./gsufsort FILE [--sa [w]] [--sa [w]] [--lcp [w]] [--da [w]] [--ligth] [--gsa [w1] [w2]] [--gesa [w1] [w2] [w3]] [--bwt] [--bin] [--docs d] [--print [p]] [--lcp_max] [--lcp_max_text] [--lcp_avg] [--trlcp [k]] [--output out]
```

_Notes:_ 

- Strings are separated per '\0' (new line) in _.txt_.

- gsufsort supports **ASCII alphabet**, with values _0_ and _1_ reserved.

- For inputs **larger than 2GB**, _gsufsort-64_ uses **21N bytes** to compute SA, LCP and DA, while its _lightweight_ version (option ``--light``) uses **17N bytes**.

Available options:

```sh
--build	              (default)
--load                load from disk FILE[.sa][.da][.lcp][.gsa][.bin]
--sa    [w]           computes SA  using w (def 4) bytes (FILE.w.sa)
--lcp   [w]           computes LCP (FILE.w.lcp)
--da    [w]           computes DA  (FILE.w.da)
--gsa   [w1][w2]      computes GSA=(text, suff) using pairs of (w1, w2) bytes (FILE.w1.w2.gsa)
--light               runs lightweight algorithm to compute DA (also GSA)
--gesa  [w1][w2][w3]  computes GESA=(GSA, LCP, BWT) (FILE.w1.w2.w3.1.gesa)
--bwt                 computes BWT using 1 byte (FILE.1.bwt)
--bin                 computes T^{cat} (FILE.1.bin)
--docs    d           number of strings (def all FILE)
--print   [p]         print arrays (stdout) A[1,min(p,N)]
--lcp_max             outputs maximum LCP-value
--lcp_max_text        outputs maximum LCP-value (text)
--lcp_avg             outputs average LCP-value
--trlcp   k           outputs k-truncated LCP array (FILE.w.lcp)
--time                outputs time (in seconds)
--output  outfile     renames output file
--verbose             verbose output
--help                this help message
```

## quick test

To run a test with docs=3 strings from dataset/input-10000.txt, type:

```sh
./gsufsort dataset/input-10000.txt --docs 3 --sa --bwt
```

```sh
## store_to_disk ##
dataset/input-10000.txt.4.sa	72 bytes (n = 18)
dataset/input-10000.txt.1.bwt	18 bytes (n = 18)
malloc_count ### exiting, total: 32,859, peak: 21,420, current: 1,024
```

To see the result (--print 10) stored in disk FILE.4.sa and FILE.1.bwt, use **--load** option:

```sh
./gsufsort dataset/input-10000.txt --sa --bwt --load --print 10
```

```sh
## load_from_disk ##
dataset/input-10000.txt.4.sa	76 bytes (n = 19)
dataset/input-10000.txt.1.bwt	19 bytes (n = 19)
i	SA	BWT
0	6	a
1	12	a
2	17      n
3	5       n
4	11	b
5	9	n
6	15	n
7	3	n
8	7	$
9	13	$
malloc_count ### exiting, total: 10,438, peak: 5,790, current: 1,024
```

## authors

* [Felipe Louza](https://github.com/felipelouza)
* [Guilherme Telles](https://github.com/gptelles)
* [Simon Gog](https://github.com/simongog)
* [Nicola Prezza](https://github.com/nicolaprezza)
* [Giovanna Rosone](https://github.com/giovannarosone/)

## remarks

* The optimal algorithm [gsaca-k](https://github.com/felipelouza/gsa-is) is at the core of _gsufsort_ to compute SA, LCP and DA.


## thanks

Thanks to [Antonis Maronikolakis](https://github.com/antmarakis) by helpful suggestions.

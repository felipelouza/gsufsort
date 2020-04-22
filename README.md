# gsufsort: 

This software builds SA, LCP, DA, GSA and BWT for **string collections**. 

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
- For inputs **larger than 2GB**, use _gsufsort-64_

- Supported extensions are _.txt_, _.fasta_ and _.fastq_.

- Strings are separated per '\0' (new line) in _.txt_.

- gsufsort supports **ASCII alphabet**, with values _0_ and _1_ reserved.

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

## remarks

* The algorithm [gsaca-k](https://github.com/felipelouza/gsa-is) is at the core of gsufsort to compute SA, LCP and DA.
* For inputs larger than 2GB, _gsufsort_ uses 21N bytes to compute SA, LCP and DA, while its _lightweight_ version (option ``--light``) uses 17N bytes.

## thanks

Thanks to [antmarakis](https://github.com/antmarakis) by helpful suggestions.

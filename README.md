# gsufsort: 

This software builds SA, LCP, DA, GSA and BWT for **string collections** using gsacak algorithm \[1\].

## install

```sh
git clone https://bitbucket.org/louza/gsufsort/ 
cd gsufsort
make compile
```

## run

Given a string collection in a single file FILE.

```sh
./gsufsort FILE [--sa [w]] [--sa [w]] [--lcp [w]] [--da [w]] [--gsa [w1] [w2]] [--gesa [w1] [w2] [w3]] [--bwt] [--bin] [--docs d] [--print p] [--output out]
```

Available options:

```sh
--build	              (default)
--load                load from disk FILE[.sa][.da][.lcp][.gsa][.bin]
--sa    [w]           computes SA  using w (def 4) bytes (FILE.w.sa)
--lcp   [w]           computes LCP (FILE.w.lcp)
--da    [w]           computes DA  (FILE.w.da)
--gsa   [w1][w2]      computes GSA=(text, suff) using pairs of (w1, w2) bytes (FILE.w1.w2.gsa)
--gesa  [w1][w2][w3]  computes GESA=(GSA, LCP, BWT) (FILE.w1.w2.w3.1.gesa)
--bwt                 computes BWT using 1 byte (FILE.1.bwt)
--bin                 computes T^{cat} (FILE.1.bin)
--docs    d           number of strings (def all FILE)
--print   p           print arrays (stdout) A[1,p]
--output  outfile     renames output file
--verbose             verbose output
--help                this help message
```
_Notes:_ 
- Supported extensions are _.txt_, _.fasta_ and _.fastq_.

## quick test

To run a test with docs=3 strings from dataset/input-10000.txt, type:

```sh
./gsufsort dataset/input-10000.txt --docs 3 --sa --bwt
```

```sh
## store_to_disk ##
dataset/input-10000.txt.4.sa	76 bytes (n = 19)
dataset/input-10000.txt.1.bwt	19 bytes (n = 19)
malloc_count ### exiting, total: 24,676, peak: 13,209, current: 1,033
```

To see the result (--print 10) stored in disk FILE.4.sa and FILE.1.bwt, type

```sh
./gsufsort dataset/input-10000.txt --sa --bwt --print 10
```

```sh
## load_from_disk ##
dataset/input-10000.txt.4.sa	76 bytes (n = 19)
dataset/input-10000.txt.1.bwt	19 bytes (n = 19)
i	SA	BWT
0	18	$
1	6	a
2	12	a
3	17      n
4	5       n
5	11	b
6	9	n
7	15	n
8	3	n
9	7	$
malloc_count ### exiting, total: 10,424, peak: 5,776, current: 1,033
```

## references

\[1\] 
Louza, Felipe A., Gog, Simon, Telles, Guilherme P. (2017). 
Inducing enhanced suffix arrays for string collections. 
Theor. Comput. Sci. 678: 22-39, [github](https://github.com/felipelouza/gsa-is).


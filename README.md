# gsufsort

gsufsort \[1\] is a fast, portable and lightweight tool for constructing the
**suffix array** and related data structures for **string collections**.

gsufsort runs in internal memory and data structures are written to disk. 

For a string collection, gsufsort can compute the following data structures:

- [x] Suffix array (SA)
- [x] Inverse suffix array (ISA)
- [x] LCP-array (LCP)
- [x] k-truncated LCP-array (k-LCP)
- [x] Document array (DA)
- [x] Burrows-Wheeler transform (BWT)
- [x] Inverse BWT
- [x] Generalized suffix array (GSA)
- [x] Generalized enhanced suffix array (GESA)

## Compilation and installation

gsufsort will compile in systems with a standard C compiler (like gcc) and make. 

```sh
git clone https://github.com/felipelouza/gsufsort.git
cd gsufsort
make 
```

Issuing these commands will build executables gsufsort and gsufsort-64.

For inputs larger than **2GB**, **_gsufsort-64_** must be used.

To enable support to compressed files, zlib is required.  If zlib is
not installed in you system, build with option ``make GZ=0``.



## Execution

```sh
./gsufsort INPUT [options]
```

where INPUT is a single file or directory with a string collection.

### Construction options:

```sh
--build	              (default)
--sa    [w]           compute the SA using w bytes (default 4), write to INPUT.w.sa
--isa   [w]           compute the ISA, write to INPUT.w.isa
--lcp   [w]           compute the LCP, write to INPUT.w.lcp
--trlcp k             compute the k-truncated LCP array, write to INPUT.w.lcp
--da    [w]           compute the DA, write to INPUT.w.da
--gsa   [w1][w2]      compute the GSA=(string,suffix) as pairs of w1+w2 bytes, write to INPUT.w1.w2.gsa
--gesa  [w1][w2][w3]  compute the GESA=(GSA,LCP,BWT), write to INPUT.w1.w2.w3.1.gesa
--bwt                 compute the BWT using 1 byte per symbol, write to INPUT.bwt
--docs  d             process only the first d strings in the collection
--light               run the lightweight algorithm to compute DA, GSA and GESA
--output DIR/NAME     write output files to DIR and use NAME as a prefix for file names
```

### Loading options:

```sh
--load                load data-structures from disk INPUT[.sa][.da][.lcp][.gsa][.str]
--ibwt                invert the BWT, given INPUT.bwt, write output to INPUT.bwt.ibwt
```

### Input options:

```sh
--txt                 handle input as raw text files (one string per line)
--fasta               handle input as fasta files 
--fastq               handle input as fastq files
--dir                 include all files at the input directory
--lower               convert input to lowercase before data structures construction
--upper               convert input to uppercase before data structures construction
```

### Output options:

```sh
--str                 write the collection concatenation (T^{cat}) to INPUT.1.str
--print [p]           print the first p elements of arrays to stdout, defaults to the collection length
--qs                  write QS sequences in fastq permuted according to the BWT to INPUT.bwt.qs
--lcp_max             print maximum LCP value
--lcp_max_text        print maximum LCP value (text order)
--lcp_avg             print average LCP value
--time                print the running time in seconds
--verbose             verbose output
--help                this help message
```


## Input files 

- File types (text, fasta or fastq) will be selected by extensions:
  _.txt_, _.fasta_ (or _.fa_, _.fna._) and _.fastq_ (or _.fq_).

- Options ``--txt``, ``--fasta`` and ``--fastq`` enable loading file disregarding extensions.

- In _txt_ files, each line is taken as a strings in the collection.
  In _fasta_ and _fastq_ files, each sequence is taken as a string in the
  collection.

- gsufsort supports the ASCII alphabet, but values _0_ and _255_ are
  reserved and must not occur in the input.

- IUPAC symbols and 'N' are not handled as special symbols in _fasta_
  or _fastq_ files.

- gzipped input files (with _.gz_ extension) are supported using
  [zlib](https://github.com/felipelouza/gsufsort/tree/master/external/zlib)
  and
  [kseq](https://github.com/felipelouza/gsufsort/tree/master/external/kseq)
  libraries.  If _zlib_ is not installed in your system, build
  gsufsort with the option ``make GZ=0``.  If _zlib_ is not available
  and a gzipped file is given as input, a runtime error will occur.

- A directory may be given as input, selecting option ``--dir``.
  Every file with expected extensions in the directory will be
  processed to compose the collection, and the default output file
  prefix will be **all**.
  See also
    [Wiki](https://github.com/felipelouza/gsufsort/wiki/Multiple-Files).


## Output files 

- Output files are written by default in the current directory.

- If option ``--output DIR/`` is set, files are written to directory
  ``DIR``.  Setting ``--output DIR/NAME`` will make files be written
  to directory ``DIR`` with suffix ``NAME``.

- Output files format is discussed [below](https://github.com/felipelouza/gsufsort#output).

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

### output format

- SA, ISA, LCP, k-LCP and DA are each written sequentially to a binary
  file.  The file has no header and every integer takes ``w``
  bytes. The default value of ``w`` is 4.

- BWT and iBWT are written in ASCII format, using 1 byte per input symbol.

- The GSA is written sequentially to a binary file, with no headers.
  The values of DA and SA are intercalated throughout the file with w1 and w2 bytes respectively:
  DA[0],SA[0],DA[1],SA[1],...

- The GESA is written sequentially to a binary file, with no headers.
  The values of DA and SA, LCP and BWT are intercalated throughout the file with w1, w2, w3 and 1 bytes respectively:
  DA[0],SA[0],LCP[0],BWT[0],DA[1],SA[1],LCP[1],BWT[1]...


## wiki

See more details and additional features in [Wiki](https://github.com/felipelouza/gsufsort/wiki).

## authors

* [Felipe Louza](https://github.com/felipelouza)
* [Guilherme Telles](https://github.com/gptelles)
* [Simon Gog](https://github.com/simongog)
* [Nicola Prezza](https://github.com/nicolaprezza)
* [Giovanna Rosone](https://github.com/giovannarosone/)

## thanks

We thank to [Antonis Maronikolakis](https://github.com/antmarakis) for his helpful suggestions.

## references

\[1\] Louza, F.A., Telles, G.P., Gog, S., Prezza, N., Rosone, G.. gsufsort: constructing suffix arrays, LCP arrays and BWTs for string collections. Algorithms Mol Biol 15, 18 (2020). https://doi.org/10.1186/s13015-020-00177-y


---

<small> Supported by the project Italian MIUR-SIR [CMACBioSeq][240fb5f5] ("_Combinatorial methods for analysis and compression of biological sequences_") grant n.~RBSI146R5L. P.I. Giovanna Rosone</small> [↩](#a1)

[240fb5f5]: http://pages.di.unipi.it/rosone/CMACBioSeq.html


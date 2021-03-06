## Datasets

| Dataset         | size (GB) | sigma | documents   | avg docLen | max docLen | avg LCP | max LCP | Download Link                                                      |
|-----------------|-----------|--------|-------------|------------|------------|---------|---------|--------------------------------------------------------------------|
| shortreads      | 16.00     | 5      | 171,798,687 | 100        | 100        | 32.87   | 100     | https://drive.google.com/open?id=199dUcf-NgCV4WaWTs96siJtibd0GsDM2 |
| longreads       | 16.00     | 6      | 57,266,226  | 300        | 300        | 91.29   | 300     | https://drive.google.com/open?id=1uck1L79ERqkX4G26\_-3LYYlGkw0r2Qxe |
| pacbio          | 16.00     | 5      | 1,884,492   | 9,117      | 71,561     | 19.08   | 3,084   | https://drive.google.com/open?id=1JER4Ci1DyZtQERqILNbrebWBXQdVQrW4 |
| pacbio.1000     | 16.00     | 5      | 17,179,866  | 1,000      | 1,000      | 18.67   | 876     | https://drive.google.com/open?id=1ehqbYJmRedwiR2iLMYEP1TerkvxhhXZV |
| uniprot\_trembl* | 16.04     | 25     | 46,060,815  | 374        | 74,488     | 99.24   | 74,293  | https://www.uniprot.org/                         |
| gutenberg       | 15.88     | 255    | 344,346,526 | 50         | 757,936    | 18.97   | 9,060   | http://www.gutenberg.org/                                          |

\* release 2019\_04

## Results

|                |           |        |       | gsufsort |          |             | gsufsort-light |          |             | mkESA   |          |             |
|----------------|-----------|--------|-------|----------|----------|-------------|----------------|----------|-------------|---------|----------|-------------|
| Dataset        | size (GB) | sigma | d     | time     | RAM (GB) | total space | time           | RAM (GB) | total space | time    | RAM (GB) | total space |
| shortreads     | 16.00     | 5      | 171.8 | **4:25:52**  | 336.00   | 21.00       | 5:30:54        | **272.00**   | **17.00**       | 4:51:48 | 274.73   | 17.17       |
| longreads      | 16.00     | 6      | 57.3  | **5:00:27**  | 336.00   | 21.00       | 5:10:04        | **272.00**   | **17.00**       | 5:44:58 | 280.68   | 17.54       |
| pacbio         | 16.00     | 5      | 1.9   | **4:19:37**  | 336.04   | 21.00       | 4:54:21        | **272.03**   | **17.00**       | 4:26:39 | 272.58   | 17.03       |
| pacbio.1000    | 16.00     | 5      | 17.2  | **4:28:22**  | 336.00   | 21.00       | 5:20:39        | **272.00**   | **17.00**       | 4:44:50 | 272.32   | 17.02       |
| uniprot\_trembl | 16.04     | 25     | 46.1  | **5:11:33**  | 336.90   | 21.00       | 5:25:37        | **272.73**   | **17.00**       | 9:58:03 | 295.69   | 18.43       |
| gutenberg      | 15.88     | 255    | 344.3 | **4:17:52**  | 334.4    | 21.06       | 4:53:05        | **269.90**   | **17.00**       | -       | -        | -           |



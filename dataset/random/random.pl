#!/usr/local/bin/perl -w

if ($#ARGV == 1) {
  $d = int($ARGV[0]);
  $len = int($ARGV[1]);
}
else {
  die "usage: $0 d length\n";
}

#@S = split('','ABCDEFGHIKLMNPQRSTVWXYZ');
@S = split('','ACGT');
$l = @S;

for($j=0; $j<$d; $j++){
  print ">$j\n";
  for ($i=0; $i<$len; $i++) {
    print $S[int(rand($l))];
  }
  print "\n";
}

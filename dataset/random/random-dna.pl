#!/usr/local/bin/perl -w

if ($#ARGV == 1) {
  $d = int($ARGV[0]);
  $len = int($ARGV[1]);
}
else {
  die "usage: $0 d length\n";
}

@S = split('','ACGT');
$l = $#S+1;

for($j=0; $j<$d;$j++){
  print ">$j\n";
  $s = "";
  for ($i=0; $i<$len; $i++) {
    $aux = int(rand()*$l);
    $s .= $S[$aux];
  }
  print "$s\n";
}


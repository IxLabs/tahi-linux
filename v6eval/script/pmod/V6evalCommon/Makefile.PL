use ExtUtils::MakeMaker;

# require 5.002 ;
my $Makefile	= 'Makefile.tmp';

WriteMakefile(
	'NAME'		=> 'V6evalCommon',
	'VERSION'	=> '1.00',
	'MAKEFILE'	=> $Makefile,
	'linkext'	=> { 'LINKTYPE' => '' },
	'dist'		=> {
				'COMPRESS'	=> 'gzip',
				'SUFFIX'	=> 'gz'
			},
);

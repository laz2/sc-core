#!/usr/bin/perl -w
# this is quite dirty and quite buggy piece of code. Use with care !
# on Win32 be careful with directory delimiter "/"

use strict;

my ($list_lexems) = "";
my ($debug_parser) = "";

my ($filename);

if (!@ARGV) {
	print "Constraint code generator\n";
	exit 0;
}

$filename = $ARGV[0];
my ($directory) = $filename =~ m/^(.*\/)/;
$directory = "" unless defined $directory;

#print $directory,"\n";

open INP,$filename;

# lexer section
# lexer should be accessed only through get_lexem(), peek_lexem(), is_lex_end()
my (@lexems);
my ($clex) = 0;

# only two quotes actually supported
# quote 1 is writed after head of file and before any structures
# quote 2 is writed right to the end of file
my ($quote_1,$quote_2) = ("","");

# first quote goes first
while (<INP>) {
	last if m/^%%/;
	$quote_1 .= $_;
}

# then definitions go
while (<INP>) {
	my ($string,$param) = $_;
	last if m/^%%/;
	$string =~ s/^([^#]*)#.*$/$1/gs; # cut off comments
	push @lexems,$string =~ m/[a-zA-Z_][a-zA-Z0-9_.]*|[!,=:{}()]/g;
}

# quote 2 at last
while (<INP>) {
	$quote_2 .= $_;
}

if ($list_lexems) {
	print "List of lexems\n";
	for (@lexems) { print "$_\n"; }
}
close INP;


sub get_lexem
{
	die if is_lex_end();
	print "Getting lexem $lexems[$clex]\n" if $debug_parser;
	return $lexems[$clex++];
}

sub is_lex_end
{
	return not defined ($lexems[$clex]);
}

sub peek_lexem
{
	die if is_lex_end();
	print "Peeking lexem $lexems[$clex]\n" if $debug_parser;
	return $lexems[$clex];
}


# parser section
my ($i,$output_c);
# my (%filter,%func,%constr);
my (%constr);
my (%deconstr);
my ($filters_done,$constr_done,$func_done) = ("","","");

sub process_simple
{
	my ($hash_ref) = @_;
	while (peek_lexem() ne "}") {
		my ($name,$define);
		$name = get_lexem();
		die unless (get_lexem() eq "=");
		$define = get_lexem();
		die "duplicate entry" if (defined ($hash_ref->{$name}));
		$hash_ref->{$name} = $define;
	}
}

# sub process_functions
# {
# 	process_simple(\%func);
# }

# sub process_filters
# {
# 	process_simple(\%filter);
# }

sub process_constraints
{
	while (peek_lexem() eq "constraint") {
		get_lexem();
		my ($name,$lexem,$define,@params,@fix);
		$name = get_lexem();
		die if defined ($constr{$name});
		die unless (get_lexem eq "{");
		while (($lexem = get_lexem()) ne "}") {
			if ($lexem eq "define") {
				$define = get_lexem();
			} elsif ($lexem eq "param") {
				my $lex;
				push @params, ($lex = get_lexem());
				if (peek_lexem() eq "!") {
					get_lexem();
					die "incorrect '!' param" if $lex ne "sc_addr" and $lex ne "sc_segment" and $lex ne "sc_boolean";
					push @fix, ((scalar @params) - 1);
				}
			} else {
				die "unexpected lexem &peek_lexem()";
			}
		}
		
		$constr{$name} = {
			define => $define,
			params => \@params,
			fix => \@fix
		};
	}
}

sub read_list
{
	my (@list,$lexem);
	push @list, get_lexem();
	while (peek_lexem() eq ",") {
		get_lexem();
		push @list, get_lexem();
	}
	return \@list;
}

sub read_io_list
{
	my (@ilist,@olist,$lexem);
	if (peek_lexem() ne ":") {
		push @ilist, get_lexem();
		while (peek_lexem eq ",") {
			get_lexem();
			if (peek_lexem() eq "," || peek_lexem() eq ":") {
				push @ilist,"";
				next;
			}
			push @ilist, get_lexem();
		}
	}
	if (peek_lexem() eq ":") {
		get_lexem();
		if (peek_lexem() ne ",") {
			push @olist, get_lexem();
		} else {
			push @olist,"";
		}
		while (peek_lexem eq ",") {
			get_lexem();
			if (peek_lexem() eq ",") {
				push @olist,"";
				next;
			}
			push @olist, get_lexem();
		}
	}
	return (\@ilist, \@olist);
}

# %deconstr = (
# 	"s3_f_a_d_def" => {
# 		input => [ "e1","t2","t3","e3","d3" ],
# 		output => [ "e1","e2","e3" ],
# 		var => [ "r2","r3" ],
# 		prg => [
# 			{
# 				type => "constr",
# 				name => "arc_type",
# 				input => [ "e1", "t2"],
# 				output => [ "", "r2"]
# 			}, {
# 				type => "func",
# 				name => "arc_end",
# 				input => [ "r2" ],
# 				output => [ "r3" ]
# 			}, {
# 				type => "filter",
# 				name => "eq_with_d",
# 				input => [ "r3", "e3", "t3", "d3" ]
# 			}
# 		]
# 	}
# );

sub process_deconstr
{
	my ($name);
	my (%rec) = (
		input => [],
		var => [],
		prg => []
	);
	$name = get_lexem();
	die "duplicated deconstr $name" if (defined ($deconstr{$name}));
	die if (get_lexem() ne "{");
	while (peek_lexem() ne "}") {
		my ($lexem) = get_lexem();
		if ($lexem eq "input") {
			$rec{input} = read_list();
			next;
		}
		if ($lexem eq "var") {
			$rec{var} = read_list();
			next;
		}
		if ($lexem eq "constr" || $lexem eq "filter" || $lexem eq "func") {
			my (%prec) = (
				type => $lexem,
				name => &get_lexem()
			);
			($prec{input}, $prec{output}) = read_io_list();
			push @{$rec{prg}}, \%prec;
		}
		if ($lexem eq "return") {
			my (%prec) = (
				type => $lexem
			);
			$prec{input} = read_list();
			push @{$rec{prg}}, \%prec;
		}
	}
	$deconstr{$name} = \%rec;
}

sub process_deconstr_all
{
	while (peek_lexem() eq "deconstr") {
		get_lexem();
		process_deconstr();
		die unless (get_lexem() eq "}");
	}
}

while (not is_lex_end()) {
	my ($lexem) = get_lexem();
	if ($lexem eq "c_output") {
		die unless (get_lexem() eq "(");
		$output_c = get_lexem();
		die unless (get_lexem() eq ")");
		next;
	}
	# if ($lexem eq "filter_section") {
	# 	die unless (get_lexem() eq "{");
	# 	process_filters();
	# 	die unless (get_lexem() eq "}");
	# 	next;
	# }
	# if ($lexem eq "function_section") {
	# 	die unless (get_lexem() eq "{");
	# 	process_functions();
	# 	die unless (get_lexem() eq "}");
	# 	next;
	# }
	if ($lexem eq "constr_section") {
		die unless (get_lexem() eq "{");
		process_constraints();
		die unless (get_lexem() eq "}");
		next;
	}
	if ($lexem eq "deconstr_section") {
		die unless (get_lexem() eq "{");
		process_deconstr_all();
		die unless (get_lexem() eq "}");
		next;
	}
	die "parser error";
}

# writer section

$output_c = $directory . $output_c unless ( $output_c =~ /^\//);
#print "opening $output_c\n";

open OUTPUT, ">$output_c" || die "cannot open $output_c for write";

print OUTPUT <<'EOF';
/*
 * This is machine generated file don't edit it by hand !
 */

EOF
#'
my (%dec);

print OUTPUT $quote_1;
#%dec = (
#	"s3_f_a_d_def" => {
#		input_cnt => 5,
#		var_cnt => 2,
#		output_cnt => 3
#	}
#);

sub get_index # ($item, \@array)
{
	my ($item,$aref) = @_;
	for my $i (0..scalar @{$aref}-1) {
		return $i if ($aref->[$i] eq $item);
	}
	return -1;
}

sub translate_prm
{
	my ($item,$ref) = @_;
	my $index;
	return -1 if ($item eq "");
	$index = get_index($item,$ref->{input});
	if ($index != -1) {
		return $index;
	}
	$index = get_index($item,$ref->{var});
	if ($index != -1) {
		return $index + scalar @{$ref->{input}};
	}
	die "cannot find index of \"$item\"";
}

my %type_mapping = (
	"constr" => "SCD_CONSTR",
	"filter" => "SCD_FILTER",
	"func" => "SCD_FUNC",
	"return" => "SCD_RETURN"
);

# first step
foreach my $i (keys %deconstr) {
	my (%rec) = (
		input_cnt => scalar @{$deconstr{$i}{input}},
		var_cnt => scalar @{$deconstr{$i}{var}},
		prg => []
	);
	foreach my $drecref (@{$deconstr{$i}{prg}}) {
		my $type = $drecref->{type};
		my (%prec) = (
			type => $type_mapping{$type},
			input => [],
			output => []
		);
		# if ($type eq "constr") {
		# 	$define = $constr{$name}{define};
		# } elsif ($type eq "filter") {
		# 	$define = $filter{$name};
		# } elsif ($type eq "func") {
		# 	$define = $func{$name};
		# }
		#die "unknown element $define of type $type" if (not defined $define);
		#$prec{define} = $define;
		if (defined ($drecref->{name})) {
			$prec{name} = $drecref->{name};
		}
		foreach my $k (@{$drecref->{input}}) {
			push @{$prec{input}},
					translate_prm($k,$deconstr{$i});
		}
		foreach my $k (@{$drecref->{output}}) {
			push @{$prec{output}},
					translate_prm($k,$deconstr{$i});
		}
		push @{$rec{prg}}, \%prec;
	}
	$dec{$i} = \%rec;
}

sub write_ints # ($name,\@array)
{
	my ($name,$aref) = @_;
	my $col = 0;
	my $first = 1;
	return if (scalar @$aref == 0);
	print OUTPUT "static\tint ".$name."[] = {\n\t";
	for my $i (@$aref) {
		if (not $first) {
			if ($col++ == 8) {
				print OUTPUT ",\n\t";
				$col = 0;
			} else {
				print OUTPUT ",\t";
			}
		}
		$first = 0;
		print OUTPUT $i;
	}
	print OUTPUT "\n};\n";
}

#write_ints("test",[1 .. 100]);

sub write_list { for (@_) { print $_,"\n"; } }

# sub write_prg_item # ($name,\%item)
# {
# 	my ($name,$rref) = @_;
# #	write_list(%$rref);
# 	write_ints($name."_i",$rref->{input});
# 	write_ints($name."_o",$rref->{output});
# 	print "static struct sc_deconstruct_prg_item $name = {\n";
# 	print "\t".$rref->{type}.",\n";
# 	print "\t".scalar @{$rref->{input}}.",\n";
# 	print "\t".$name."_i,\n";
# 	print "\t".scalar @{$rref->{output}}.",\n";
# 	print "\t".$name."_o,\n";
# 	print "\t".$rref->{define}."\n};\n";
# }

sub write_prg # ($name,\@prg)
{
	my ($name,$pref) = @_;
	for my $i (0..scalar @$pref-1) {
		write_ints($name."_i".$i."_i",$pref->[$i]->{input});
		write_ints($name."_i".$i."_o",$pref->[$i]->{output});
	}
	print OUTPUT "static struct sc_deconstruct_prg_item ".$name."[] = {\n";
	my $cnt=0;
	foreach my $i (@$pref) {
		my $iname = $name . "_i" . $cnt;
		if ($cnt == 0) {
			print OUTPUT "\t{\n";
		} else {
			print OUTPUT ", {\n";
		}
		print OUTPUT "\t\t".$i->{type}.",\n";
		print OUTPUT "\t\t".scalar @{$i->{input}}.",\n";
		if (scalar @{$i->{input}} == 0) {
			print OUTPUT "\t\t0,\n";
		} else {
			print OUTPUT "\t\t".$iname."_i,\n";
		}
		print OUTPUT "\t\t".scalar @{$i->{output}}.",\n";
		if (scalar @{$i->{output}} == 0) {
			print OUTPUT "\t\t0,\n";
		} else {
			print OUTPUT "\t\t".$iname."_o,\n";
		}
		if (defined $i->{name}) {
			print OUTPUT "\t\t\"".$i->{name}."\"\n";
		} else {
			print OUTPUT "\t\t0\n";
		}
		
		print OUTPUT "\t}";
		$cnt++;
	}
	print OUTPUT "\n};\n";
}

#die unless defined ($dec{"s3_f_a_d_def"}{prg});
#write_prg("s3_f_a_d_def_prg",$dec{"s3_f_a_d_def"}{prg});

my %params_mapping = (
	"sc_addr"    => "SC_PD_ADDR",
	"sc_addr_0"  => "SC_PD_ADDR_0",
	"sc_type"    => "SC_PD_TYPE",
	"int"        => "SC_PD_INT",
	"sc_int"     => "SC_PD_INT",
	"sc_segment" => "SC_PD_SEGMENT",
	"sc_boolean" => "SC_PD_BOOLEAN",
	"boolean"    => "SC_PD_BOOLEAN"
);

sub write_input_params # ($name,\@params)
{
	my ($name,$paref) = @_;
	print OUTPUT "static\tsc_param_spec\t",$name,"[]={\n\t";
	my $v = shift @$paref;
	die "don't want to write empty spec!" if (!defined $v);
	print OUTPUT $params_mapping{$v};
	foreach my $i (@$paref) {
		print OUTPUT ",",$params_mapping{$i};
	}
	print OUTPUT "\n};\n";
}

# writing constraint infos
foreach my $i (keys %constr)
{
	write_input_params($i."_ispec",$constr{$i}{params});
	die "constraint $i have no fix params" if (@{$constr{$i}{fix}} == 0);
	write_ints($i."_fix",$constr{$i}{fix});
	print OUTPUT "static struct sc_constraint_info ",$i," = {\n\t0,\n";
	print OUTPUT"\t\"$i\",\n";
	# don't know why +1
	print OUTPUT"\t",scalar @{$constr{$i}{params}} + 1 ,",\n";
	print OUTPUT"\t",$i,"_ispec,\n";
	print OUTPUT"\t",scalar @{$constr{$i}{fix}},",\n";
	print OUTPUT"\t",$i,"_fix\n";
	print OUTPUT"};\n";
}

# writing deconstructs
foreach my $i (keys %dec) {
	write_prg($i . "_prg",$dec{$i}{prg});
	print OUTPUT "static struct sc_deconstruct $i = {\n";
	print OUTPUT "\t".$dec{$i}{input_cnt}.",\n";
	print OUTPUT "\t".$dec{$i}{var_cnt}.",\n";
	print OUTPUT "\t".(scalar @{$dec{$i}{prg}}).",\n";
	print OUTPUT "\t".$i."_prg\n};\n";
}

print OUTPUT $quote_2;
close OUTPUT;

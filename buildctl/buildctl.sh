#!/bin/bash

# ---- required subroutines
set_globals() {
    # required globals:
    g_name="ConsensusCore"

    # other globals:
    g_make_rootdir=$(readlink -f "${g_progdir}/../../../../../prebuilt.tmpout/make/make_3.81-8.2ubuntu3/usr")
    g_gcc_rootdir=$(readlink -f "${g_progdir}/../../../../../prebuilt.out/3.x/gcc/gcc-4.8.4/libc-2.5")

    g_make_cmd='
            ${g_make_rootdir}/bin/make -C "${g_progdir}/.."
                GXX="${g_gcc_rootdir}/bin/g++"
                AR="${g_gcc_rootdir}/bin/ar"
    '

    g_make_cmd=$(echo $g_make_cmd)

    g_outdir_name="_output";
    g_outdir_abs="$(readlink -m "$g_progdir/../$g_outdir_name")"
    g_outdir=$g_outdir_abs;
    g_installbuild_dir_abs="${g_outdir}/install-build"
    g_installbuild_dir="${g_installbuild_dir_abs}"
}

# ---- build targets

clean() {
    echo "Running $g_name 'clean' target..."
    # Clean the build artifacts
    eval "$g_make_cmd" clean ${1+"$@"}
    # Remove the _output directory
    rm -rf "${g_outdir}"

    bash ${g_progdir}/pkgbuild.sh --clean
}
cleanall() {
    echo "Running $g_name 'cleanall' target..."
    clean;
}
build() {
    echo "Running $g_name 'build' target..."

    # Create dependency links
    rm -rf "${g_outdir}/deplinks"
    mkdir -p "${g_outdir}/deplinks"
    ln -s "../../../../../../prebuilt.out/swig/swig-2.0.10/centos-5" "${g_outdir}/deplinks/swig"
    ln -s "../../../../../../prebuilt.out/boost/boost_1_55_0" "${g_outdir}/deplinks/boost"
    ln -s "../../../../../../prebuilt.out/python/python-2.7.3/centos-5" "${g_outdir}/deplinks/python"
    ln -s "../../../../../../prebuilt.tmpsrc/pythonpkgs/numpy/numpy_1.7.1/_output/install" "${g_outdir}/deplinks/numpy"
    ln -s "../../../../../../prebuilt.tmpout/libpcre3/libpcre3_7.4-1ubuntu2.1" "${g_outdir}/deplinks/libpcre"

    # build the C libraries
    eval "$g_make_cmd" python \
	CONFIG_MAKE="/dev/null" \
	CPP_ABI="c++98" \
	DEBUG="" \
	BOOST="${g_outdir_name}/deplinks/boost" \
	SWIG="LD_LIBRARY_PATH=${g_outdir_abs}/deplinks/libpcre/usr/lib\ ${g_outdir_name}/deplinks/swig/bin/swig" \
	SWIG_LIB="${g_outdir_name}/deplinks/swig/share/swig/2.0.10" \
	PYTHON_INCLUDE="${g_outdir_name}/deplinks/python/include/python2.7" \
        NUMPY_INCLUDE="${g_outdir_name}/deplinks/numpy/lib/python2.7/site-packages/numpy-1.7.1-py2.7-linux-x86_64.egg/numpy/core/include" \
	${1+"$@"}

    # build the python code
    bash ${g_progdir}/pkgbuild.sh --build
}
install_build() {
    if ! $opt_no_sub_targets; then
	build;
    fi

    echo "Running $g_name 'install-build' target..."

if false; then
    # clean install dir
    rm -rf "$g_installbuild_dir";
    mkdir -p "$g_installbuild_dir";

    # install libs
    mkdir "$g_installbuild_dir/lib"
    cp -a "${g_progdir}/../${g_name}.a"  "$g_installbuild_dir/lib"

    # install includes
    mkdir -p "$g_installbuild_dir/include"        
    # FIXME: Is there a better way to specify all the include headers that
    #        we need to export to other programs that depend on libblasr??
    for i in . utils statistics tuples format files suffixarray ipc bwt datastructures/anchoring datastructures/alignment datastructures/alignmentset algorithms/alignment algorithms/compare algorithms/sorting algorithms/alignment/sdp algorithms/anchoring; do
	mkdir -p "$g_installbuild_dir/include/alignment/$i"
	cp -a "${g_progdir}/../$i"/*.hpp "$g_installbuild_dir/include/alignment/$i"
    done
    for i in tuples datastructures/alignment; do
	mkdir -p "$g_installbuild_dir/include/alignment/$i"
	cp -a "${g_progdir}/../$i"/*.h "$g_installbuild_dir/include/alignment/$i"
    done
fi

    # build the python code
    bash ${g_progdir}/pkgbuild.sh --install

}
install_prod() {
    echo "Running $g_name 'install-prod' target..."
}
publish_build() {
    if ! $opt_no_sub_targets; then
	install_build;
    fi

    echo "Running $g_name 'publish-build' target..."

}
publish_prod() {
    if ! $opt_no_sub_targets; then
	install_prod;
    fi
    echo "Running $g_name 'cleanall' target..."

}


# ---- End of Module-specific code
# Common code from here on out, do not modify...

# ---- error handling
set -o errexit;
set -o posix;
set -o pipefail;
set -o errtrace;
unexpected_error() {
    local errstat=$?
    echo "${g_prog:-$(basename $0)}: Error! Encountered unexpected error at 'line $(caller)', bailing out..." 1>&2
    exit $errstat;
}
trap unexpected_error ERR;


g_prog=$(basename $0);
g_progdir=$(dirname $0);

# ---- usage

usage() {
  local exitstat=2;
  if [[ ! -z "$2" ]] ; then
      exitstat=$2;
  fi

  # Only redirect to stderr on non-zero exit status
  if [[ $exitstat -ne 0 ]] ; then
      exec 1>&2;
  fi

  if [[ ! -z "$1" ]] ; then
      echo "$g_prog: Error! $1" 1>&2;
  fi

  echo "Usage: $g_prog [--help] \\"
#  echo "              -t|--target buildtarget";
#  echo "         -t|--target     -- chef target to build (e.g. 'cookbookname::build')";
  echo "         --help          -- print this usage";
  echo "";

  # bash only:
  if [[ $exitstat -ne 0 ]] ; then
      echo "  at: $(caller)";
  fi
  exit $exitstat;
}

# ---- argument parsing

# Save off the original args, use as "${g_origargs[@]}" (with double quotes)
declare -a g_origargs;
g_origargs=( ${1+"$@"} )

opt_target_exist_check=false;
opt_no_sub_targets=false;
opt_process_all_deps=false;
declare -a opt_additional_options;
declare -a opt_targets;
while [[ $# != 0 ]]; do
    opt="$1"; shift;
    case "$opt" in
	# Flag with no argument example:
	#   --flag|--fla|--fl|--f)
	#     opt_flag=true;;
	# Option with argument example:
	#   --arg|--ar|--a)
	#     [[ $# -eq 0 ]] && usage;
	#     opt_somearg=$1; shift;;
	-e|--exists|--exist-check|--target-exist-check) opt_target_exist_check=true;;
	-s|--no-sub|--no-subs|--no-sub-targets|--single) opt_no_sub_targets=true;;
	-d|--deps|--process-all-deps|--all-deps|-all) opt_process_all_deps=true;;
	-o) 
	    [[ $# -eq 0 ]] && usage;
	    opt_additional_options=( "${opt_additional_options[@]}" "$1" );
	    shift;;
	-h|-help|--help|--hel|--he|--h) usage "" 0;;
	--*) opt_targets=( "${opt_targets[@]}" "$opt" );;
	-*) usage "Unrecognized option: $opt";;
	*)  usage "Extra trailing arguments: $opt $@";;
    esac
done

# ---- error functions
merror() {
    echo "$g_prog: Error! ""$@" 1>&2;
    exit 1;
}
minterror() {
    echo "$g_prog: Internal Error! ""$@" 1>&2;
    exit 1;
}
mwarn() {
    echo "$g_prog: Warning! ""$@" 1>&2;
}

# ---- globals

# ---- subroutines

munge_target() {
    local target=$1; shift;
    local mtarget=$target;
    
    mtarget=${mtarget#--}
    mtarget=${mtarget//-/_}
    echo "$mtarget"
}

# ---- main

set_globals;

warnings=false;
for target in "${opt_targets[@]}"; do
    mtarget=$(munge_target "$target");
    if ! declare -f -F "$mtarget" > /dev/null; then
	if $opt_strict; then
	    mwarn "target '$target' does not exist"
	    warnings=true;
	else
	    echo "$g_prog: target '$target' does not exist"
	fi
    fi
done
if $warnings; then
    merror "Detected warnings, bailing out..."
fi	

if ! $opt_target_exist_check; then
    for target in "${opt_targets[@]}"; do
	mtarget=$(munge_target "$target");
	eval "$mtarget" "${opt_additional_options[@]}"
    done
fi

exit 0;


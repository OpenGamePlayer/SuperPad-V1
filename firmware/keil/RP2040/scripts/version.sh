HEADER_PATH="src/headers/version.h"

if ( git describe --tags 2>/dev/null ); then
    TAG=`git describe --tags`
elif [ -n "$GITHUB_REF" ]; then
    TAG=${GITHUB_REF}
else
    # Local build without tags: fall back to a sane default instead of an
    # empty VERSION (matches PlatformIO gen_version.py behaviour).
    TAG="0.1.0"
fi

if [ -f $HEADER_PATH ]; then
    HEADER_OLD=`cat $HEADER_PATH`
else
    HEADER_OLD=''
fi

HEADER_NEW="#define VERSION \"${TAG}\""
if [ "$HEADER_NEW" != "$HEADER_OLD" ]; then
    echo "Overwriting version file"
    echo $HEADER_NEW > $HEADER_PATH
fi

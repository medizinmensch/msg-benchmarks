target=$1
if [[ !("$target" == "czmq") && !("$target" == "nanomsg") ]]; then
    echo "Compile.sh: Target was set to <$target> which is not valid. Valid options are 'nanomsg' and 'czmq'"
    return 0 2>/dev/null || exit "0"
fi

echo "This script reached the end"

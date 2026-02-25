TAG="pspsdk_host0_test"

set -xe

if [ "$REBUILD_IMAGE" == "true" ] && podman image exists $TAG
then
	podman image rm -f $TAG
fi

if ! podman image exists $TAG
then
	podman image build -f Dockerfile -t $TAG
fi

mkdir -p workdir

podman run \
	--rm -it \
	-v /dev/bus/usb:/dev/bus/usb \
	-v ./:/workdir \
	-w /workdir \
	$TAG

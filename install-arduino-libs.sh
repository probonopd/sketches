#!/bin/bash

# Install Arduino libraries from GitHub
#
# Put URLs like these into your script and the libraries will
# automatically be downloaded from GitHub if they are not
# already installed in the $LIBDIR libraries directory
# https://github.com/Makuna/NeoPixelBus/tree/DmaDriven
# https://github.com/Makuna/NeoPixelBus/archive/DmaDriven.zip
# https://github.com/adafruit/Adafruit_MQTT_Library/archive/0.12.2.zip
LIBDIR="$HOME/Arduino/libraries"

which wget >/dev/null || sudo apt-get -y install wget

if [ "$1" == "" ] ; then
  SEARCH_PATH=$(cd $(dirname $0) && pwd)
else
  SEARCH_PATH="$1"
fi
echo "Installing libraries for sketches in $SEARCH_PATH..."

FILES=$(find "$SEARCH_PATH" -name "*.ino")
TARGZURLS=""

for FILE in $FILES ; do
  # Find URLs in the format https://github.com/Makuna/NeoPixelBus/archive/DmaDriven.zip
  URLS=$(grep -Poe "(https://github.com/.*zip)" $FILE)
  for URL in $URLS; do
    USER=$(echo $URL | cut -d "/" -f 4)
    PROJECT=$(echo $URL | cut -d "/" -f 5)
    BRANCH=$(echo $URL | cut -d "/" -f 7 | cut -d "." -f 1)
    TARGZURL=$(echo "https://codeload.github.com/$USER/$PROJECT/tar.gz/$BRANCH")
    TARGZURLS="$TARGZURLS $TARGZURL"
  done
  # Find URLs in the format https://github.com/Makuna/NeoPixelBus/tree/DmaDriven
  URLS=$(grep -Poe "(https://github.com/.*tree/.*)\s*" $FILE)
  for URL in $URLS; do
    REALURL=$(curl $URL -s -L -o /dev/null -w '%{url_effective}')
    USER=$(echo $REALURL | cut -d "/" -f 4)
    PROJECT=$(echo $REALURL | cut -d "/" -f 5)
    BRANCH=$(echo $REALURL | cut -d "/" -f 7)
    TARGZURL=$(echo "https://codeload.github.com/$USER/$PROJECT/tar.gz/$BRANCH")
    TARGZURLS="$TARGZURLS $TARGZURL"
  done
done

for TARGZURL in $TARGZURLS ; do
  PROJECT=$(echo $TARGZURL | cut -d "/" -f 5)
  BRANCH=$(echo $TARGZURL | cut -d "/" -f 7)
  if [ -e "$LIBDIR/$PROJECT-$BRANCH" ] ; then
      echo "$LIBDIR/$PROJECT-$BRANCH exists; skipping"
  else
    wget "$TARGZURL" -O - | tar xfvz - -C "$LIBDIR"
  fi
done

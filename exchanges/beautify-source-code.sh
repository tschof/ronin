#!/bin/bash
find -name '*.[ch]' | xargs indent -kr --no-tabs

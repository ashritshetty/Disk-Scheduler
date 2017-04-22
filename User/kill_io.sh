#!/bin/sh

set -ex

cat output/?.pid|xargs kill

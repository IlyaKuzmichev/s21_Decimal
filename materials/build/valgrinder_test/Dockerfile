FROM quay.io/school21/alpine:latest

RUN apk --no-cache --upgrade add \
	git \
	git-lfs \
	curl \
	jq \
	bash \
	build-base \
	python3 \
	valgrind \
	cppcheck \
	alpine-sdk \
	pcre-dev \
	clang-extra-tools \
	automake \
	subunit-dev

RUN mkdir /test

RUN curl https://github.com/libcheck/check/releases/download/0.15.2/check-0.15.2.tar.gz --output /check-0.15.2.tar.gz -L
RUN tar -xf /check-0.15.2.tar.gz -C /
RUN cd /check-0.15.2 && /check-0.15.2/configure
RUN cd /check-0.15.2 && make; exit 0
RUN cd /check-0.15.2 && make check; exit 0
RUN cd /check-0.15.2 && make install

COPY . /project

WORKDIR "/project/src"

ENTRYPOINT ["/bin/bash"]

# RUN make test
# RUN valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./test.out 2> >(grep SUMMARY) 1>/dev/null
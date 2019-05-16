FROM yoshidan/ubuntu-clang-boost

RUN mkdir -p /work/reflex
ADD . /work/reflex
RUN cd /work/reflex && cmake -DCMAKE_BUILD_TYPE=Release -G "CodeBlocks - Unix Makefiles" .
RUN cd /work/reflex && cmake --build . --target all -- -j 1

CMD /work/reflex/reflex

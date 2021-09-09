vcpkg install ^
          boost-algorithm boost-accumulators boost-atomic boost-container boost-date-time boost-exception boost-filesystem boost-graph boost-log ^
          boost-program-options boost-property-tree boost-ptr-container boost-regex boost-serialization boost-system boost-test boost-thread boost-timer ^
          lz4 ^
          openexr ^
          alembic ^
          geogram ^
          eigen3 ^
          opencv[eigen,ffmpeg,webp,contrib,nonFree,cuda] ^
          openimageio[libraw,ffmpeg,freetype,opencv,gif,openjpeg,webp] ^
          ceres[suitesparse,cxsparse] ^
          cuda ^
          tbb ^
          --triplet x64-windows ^
		  --recurse
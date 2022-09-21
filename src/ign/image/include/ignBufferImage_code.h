namespace ign {
    namespace image {

		///
		///
		///
		template        <class T>
		BufferImage<T>::BufferImage():Image(), _data(NULL),_size(0,0),_numBands(0),_pixelSpace(0),_lineSpace(0),_bandSpace(0),_delete(true)
		{
			_data=NULL;
		}

		///
		///
		///
		template        <class T>
		BufferImage<T>::BufferImage(int NRows, int NLines, int NBands,T* data,int pixelSpace,int lineSpace, int bandSpace):Image(),_data(data),_size(NRows,NLines),_numBands(NBands),_pixelSpace(pixelSpace),_lineSpace(lineSpace),_bandSpace(bandSpace),_delete(false)
		{
		}

		///
		///
		///
		template        <class T>
		BufferImage<T>::~BufferImage()
		{
			bool verbose = false;
			if(verbose) std::cout<<"[BufferImage<T>::~BufferImage] "<<std::endl;
            if ((_delete) && (_data)){

				if(verbose) std::cout<<"[BufferImage<T>::~BufferImage] calling delete[] _data "<<std::endl;
#ifdef _IGN_SAFE_BUFFER_ALLOC_
                std::return_temporary_buffer(_data);
#else
                delete[] _data;
#endif
            }
			
		}

		///
		///
		///
		template <class T>
		BufferImage<T>::BufferImage(BufferImage<T> const &src):Image()
		{
			_size = src._size;
			_numBands = src._numBands;
			_pixelSpace = src._pixelSpace;
			_lineSpace = src._lineSpace;
			_bandSpace = src._bandSpace;
			_delete = true;
			size_t bufferSize = _size.first*_size.second*_numBands;
#ifdef _IGN_SAFE_BUFFER_ALLOC_
			std::pair <T*,ptrdiff_t> result = std::get_temporary_buffer<T>(bufferSize);
			//if (result.second>0)
			if ( result.second == (ptrdiff_t)bufferSize)
			{
			    _data =  result.first;
				memcpy(_data, src._data, bufferSize*sizeof(T));
			}
			else
			{
				std::return_temporary_buffer (result.first);
				//IGN_THROW_EXCEPTION("BufferImage : not enough memory to allocate buffer");
				std::ostringstream oss;
				oss << "BufferImage(C) : not enough memory to allocate buffer : " << bufferSize << " = " << _size.first << "x" << _size.second << "x" << _numBands;
				IGN_THROW_EXCEPTION(oss.str());
			}
#else
			_data = new T[bufferSize];
#endif
		}
		
		///
		///
		///
		template <class T>
		template <class T2>
		BufferImage<T>::BufferImage(BufferImage<T2> const &src):Image(),_data(NULL), _delete(true)
		{
			*this = src;
		}
		
		///
		///
		///
		template <class T>
		BufferImage<T>::BufferImage(T* data,
									ImageSize const &taille,
									int nbBands,
									int pixelSpace,
									int lineSpace,
									int bandSpace,
									bool toBeDeleted):Image(), 
		_data(data),_size(taille),_numBands(nbBands),_pixelSpace(pixelSpace),_lineSpace(lineSpace),_bandSpace(bandSpace),_delete(toBeDeleted)
		{
		}

		///
		///
		///
		template        <class T>
		BufferImage<T>::BufferImage(std::string const &fullPathName,int deZoom, bool del):Image(), _data(NULL),_size(0,0),_numBands(0),_pixelSpace(0),_lineSpace(0),_bandSpace(0)
		{
			this->initialize(fullPathName, deZoom, del);
		}

		///
		///
		///
		template        <class T>
		BufferImage<T>::BufferImage(int NRows,int NLines, int NBands, const T &defaultValue):Image(), _data(NULL),_size(0,0),_numBands(0),_pixelSpace(0),_lineSpace(0),_bandSpace(0),_delete(true)
		{
			this->initialize(NRows, NLines, NBands, defaultValue);
		}

		///
		///
		///
		template        <class T>
        void BufferImage<T>::initialize(std::string const &fullPathName,int deZoom, bool del, bool useExtension)
		{
			initialize(0,0,0);

            io::ImageInput image(fullPathName, useExtension);
			if (!image.isValid())
			{
				IGN_THROW_EXCEPTION(std::string("Impossible d'ouvrir le fichier ")+fullPathName);
			}
			int NC = static_cast<int>(image.size().first);
			int NL = static_cast<int>(image.size().second);
			int NCdZ = NC/deZoom;
			int NLdZ = NL/deZoom;

            //std::cout << "NL : "<<NL<<" deZoom : "<<deZoom<<" NLdZ : "<<NLdZ<<std::endl;
            
			//if (NCdZ*deZoom<NC) ++ NCdZ;
			//if (NLdZ*deZoom<NL) ++ NLdZ;


			int NbBands = static_cast<int>(image.numBands());
			initialize(NCdZ,NLdZ,NbBands);
			// modif GM pour le deZoom sur le ThomImageDriver (bug de la derniere ligne)
            image.read(0,0,0,NC,NLdZ*deZoom,NbBands,deZoom,(void*)(this->getPtr()),TypePixel<T>::GetType(),this->getPixelSpace(),this->getLineSpace(),this->getBandSpace());
            if (image.metaData())
            {
                _metaData.append(*image.metaData());
            }
			_delete=del ;

		}

        ///
		///
		///
		template        <class T>
		void BufferImage<T>::initialize(int NRows,int NLines, int NBands)
		{
			bool verbose = 0;
			if(verbose)	std::cout << "BufferImage<"<<ign::image::TypeToString(ign::image::TypePixel<T>::GetType())<<">::initialize" << std::endl;
			_size=ImageSize(NRows,NLines);
			_numBands=NBands;
			if(verbose)	std::cout << _size.first << "x" << _size.second << "x" << _numBands << std::endl;
            if ((_data)&&(_delete))
			{
				if (verbose) std::cout<<"delete old data at adress: "<<_data<<std::endl;
                delete[] _data;
			}
			if (NRows*NLines>0)
			{
				if(verbose)	std::cout << "Allocation du buffer" << std::endl;
#ifdef _IGN_SAFE_BUFFER_ALLOC_
				std::pair <T*,ptrdiff_t> result = std::get_temporary_buffer<T>(NRows*NLines*NBands);
				if(verbose)	std::cout << "Allocation du buffer: " <<result.second<<" | "<<NRows*NLines*NBands<<std::endl;
				if ( result.second == NRows*NLines*NBands)
				{
					_data =  result.first;
				}
				else
				{
					_data = NULL;
					std::return_temporary_buffer (result.first);
					//IGN_THROW_EXCEPTION("BufferImage : not enough memory to allocate buffer");
					std::ostringstream oss;
					oss << "BufferImage(I) : not enough memory to allocate buffer : " << NRows*NLines*NBands << " = " << NRows << "x" << NLines << "x" << NBands;
					IGN_THROW_EXCEPTION(oss.str());
				}
#else
				_data = new T[NRows*NLines*NBands];
#endif

				if (_data==NULL)
				{
					std::cout << "BufferImage : Erreur d'allocation d'un buffer de "<<NRows<<" x "<<NLines<<" x "<<NBands<<std::endl;
				}
				if(verbose)	std::cout << "Buffer correctement alloue" << std::endl;
			}
			else
            {
				if(verbose)	std::cout << "Buffer NULL " << std::endl;
                _data = NULL;
            }
            
			_pixelSpace = NBands;
			_lineSpace = NRows*NBands;
			_bandSpace = 1;
			_delete=true;
		}


		///
		///
		///
		template        <class T>
		void BufferImage<T>::initialize(int NRows,int NLines, int NBands, const T &defaultValue)
		{
            this->initialize(NRows,NLines,NBands);

			//initialisation des valeurs :
            *(this) = (T)defaultValue;
		}

		///
		///
		///
		template        <class T>
		void BufferImage<T>::crop(BufferImage<T> const &src,int x1,int y1,int x2,int y2, int clip)
		{
			int NCsrc, NLsrc;
            NCsrc = src.numCols();
            NLsrc = src.numLines();
			int x1Clip=x1, y1Clip=y1, x2Clip=x2, y2Clip=y2;
			if (clip)
			{
				if (x1Clip<0)
					x1Clip = 0;
				if (y1Clip<0)
					y1Clip = 0;
				if (x2Clip>NCsrc)
					x2Clip = NCsrc;
				if (y2Clip>NLsrc)
					y2Clip = NLsrc;
			}
			int NC = x2Clip-x1Clip;
			int NL = y2Clip-y1Clip;
			int NbB = src.numBands();
            this->initialize(NC,NL,NbB);
			for(int l=0;l<NL;++l)
			{
				for(int c=0;c<NC;++c)
				{
					for(int k=0;k<NbB;++k)
					{
						if (clip || ( ((c+x1Clip)>=0) && ((c+x1Clip)<NCsrc) && ((l+y1Clip)>=0) && ((l+y1Clip)<NLsrc) )  )
							(*this)(c,l,k) =src(c+x1Clip,l+y1Clip,k);
						else
							(*this)(c,l,k) = (T)0;
					}
				}
			}
		}

		///
		///
		///
		template        <class T>
		void BufferImage<T>::rotate(BufferImage<T> const &src)
		{
            size_t NCsrc = src.numCols();
            size_t NLsrc = src.numLines();
			size_t NbB = src.numBands();
			size_t NC = NLsrc;
			size_t NL = NCsrc;
            this->initialize(NC,NL,NbB);
			for(size_t l=0;l<NL;++l)
			{
				for(size_t c=0;c<NC;++c)
				{
					for(size_t k=0;k<NbB;++k)
					{
						(*this)(c,l,k) =src(NL-c,l,k);
					}
				}
			}
		}

		///
		///
		///
		template        <class T>
		void BufferImage<T>::cropandrotate(BufferImage<T> const &src,int x1,int y1,int x2,int y2, int clip)
		{
			int NCsrc, NLsrc;
            NCsrc = src.numCols();
            NLsrc = src.numLines();
			int x1Clip=x1, y1Clip=y1, x2Clip=x2, y2Clip=y2;
			if (clip)
			{
				if (x1Clip<0)
					x1Clip = 0;
				if (y1Clip<0)
					y1Clip = 0;
				if (x2Clip>NCsrc)
					x2Clip = NCsrc;
				if (y2Clip>NLsrc)
					y2Clip = NLsrc;
			}
			int NL = x2Clip-x1Clip;
			int NC = y2Clip-y1Clip;
			int NB = src.numBands();
            this->initialize(NC,NL,NB);
			for(int l=0;l<NL;++l)
			{
				for(int c=0;c<NC;++c)
				{
					for(int k=0;k<NB;++k)
					{
						int csrc = x1Clip+l;
						int lsrc = y2Clip-c-1;
						if (clip || ( (csrc>=0) && (csrc<NCsrc) && (lsrc>=0) && (lsrc<NLsrc) ) )
							(*this)(c,l,k) =src(csrc,lsrc,k);
						else
							(*this)(c,l,k) = (T)0;
					}
				}
			}
		}


		///
		///
		///
		template        <class T>
		void BufferImage<T>::read(int nXOff, int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, int nDeZoom, void *buffer, eTypePixel eBufferType, int nPixelSpace, int nLineSpace, int nBandSpace)const
		{
			char* charPtr = (char*)(buffer);
			size_t typeSize = ign::image::TypeSize(eBufferType);
			for(int l=0;l<nYSize;++l)
			{
				for(int c=0;c<nXSize;++c)
				{
					for(int k=0;k<nNBands;++k)
					{
						//on se place sur (c, l, k) dans buffer

						size_t shift = (nLineSpace * l + nPixelSpace * c + nBandSpace * k)*typeSize;

						void* ptr = (void*)(charPtr + shift);

						//on recupere le pixel (c+nXOff, l + nYOff, nBand0+k) dans this
						//et on le caste en eBufferType et on le copie dans ptr
						switch(eBufferType)
						{
							case numeric::eUnsignedChar:
							{
								unsigned char v=(unsigned char)(*this)(c+nXOff, l + nYOff, nBand0+k);

								::memcpy(ptr, &v, typeSize);

								break;
							}
							case numeric::eUnsignedShort:
							{
								unsigned short v=(unsigned short)(*this)(c+nXOff, l + nYOff, nBand0+k);

								::memcpy(ptr, &v, typeSize);

								break;
							}
							case numeric::eUnsignedInt:
							{
								unsigned int v=(unsigned int)(*this)(c+nXOff, l + nYOff, nBand0+k);

								::memcpy(ptr, &v, typeSize);

								break;
							}
							case numeric::eSignedShort:
							{
								signed short v=(signed short)(*this)(c+nXOff, l + nYOff, nBand0+k);

								::memcpy(ptr, &v, typeSize);

								break;
							}
							case numeric::eFloat:
							{
								float v=(float)(*this)(c+nXOff, l + nYOff, nBand0+k);

								::memcpy(ptr, &v, typeSize);

								break;
							}
							default:
								IGN_ASSERT(false);
						}
					}
				}
			}
		}

		///
		///
		///
		template        <class T>
		void BufferImage<T>::write(int nXOff,int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, void *buffer, eTypePixel eBufferType,int nPixelSpace, int nLineSpace, int nBandSpace)
		{

			size_t typeSize = ign::image::TypeSize(eBufferType);
			char* charPtr = (char*)(buffer);
			for(int l=0;l<nYSize;++l)
			{
				for(int c=0;c<nXSize;++c)
				{
					for(int k=0;k<nNBands;++k)
					{
						//on se place sur (c, l, k) dans buffer

						size_t shift = (nLineSpace * l + nPixelSpace * c + nBandSpace * k)*typeSize;

						void* ptr = (void*)(charPtr + shift);

						//on le caste en eBufferType et on le copie dans le pixel (c+nXOff, l + nYOff, nBand0+k) de this
						switch(eBufferType)
						{
							case numeric::eUnsignedChar:
							{
								unsigned char* v = (unsigned char*)(ptr);
								(*this)(c+nXOff, l + nYOff, nBand0+k) = static_cast< T >(*v);
								break;
							}
							case numeric::eUnsignedShort:
							{
								unsigned short* v = (unsigned short*)(ptr);
								(*this)(c+nXOff, l + nYOff, nBand0+k) = static_cast< T >(*v);
								break;
							}
							case numeric::eUnsignedInt:
							{
								unsigned int* v = (unsigned int*)(ptr);
								(*this)(c+nXOff, l + nYOff, nBand0+k) = static_cast< T >(*v);
								break;
							}
							case numeric::eSignedShort:
							{
								signed short* v = (signed short*)(ptr);
								(*this)(c+nXOff, l + nYOff, nBand0+k) = static_cast< T >(*v);
								break;
							}
							case numeric::eFloat:
							{
								float* v = (float*)(ptr);
								(*this)(c+nXOff, l + nYOff, nBand0+k) = static_cast< T >(*v);
								break;
							}
							default:
								IGN_ASSERT(false);
						}
					}
				}
			}

		}

		///
		///
		///
		template        <class T>
		bool	BufferImage<T>::isReadable() const
		{
			return true;
		}

		///
		///
		///
		template        <class T>
		bool	BufferImage<T>::isWritable() const
		{
			return true;
		}

		///
		///
		///
		template        <class T>
		bool	BufferImage<T>::isWritable(std::string nomImage) const
		{
            typename BufferImage<T>::BufferImage bufferImage(nomImage);

			return bufferImage.isWritable();
		}

		///
		///
		///
		template        <class T>
		bool	BufferImage<T>::isInRAM() const
		{
			return true;
		}

		///
		///
		///
        template        <class T>
		bool							BufferImage<T>::isTiled() const
		{
			return false;
		}

		///
		///
		///
		template        <class T>
		T* BufferImage<T>::getPtr()
		{
			return _data;
		}

		///
		///
		///
		template        <class T>
		T const * BufferImage<T>::getPtr()const
		{
			return _data;
		}

		///
		///
		///
		template        <class T>
		int BufferImage<T>::getPixelSpace()const
		{
			return _pixelSpace;
		}

		///
		///
		///
		template        <class T>
		int BufferImage<T>::getLineSpace()const
		{
			return _lineSpace;
		}

		///
		///
		///
		template        <class T>
		int BufferImage<T>::getBandSpace()const
		{
			return _bandSpace;
		}

#ifndef IGN_WITH_DEBUG
#define IGN_WITH_DEBUG 0
#endif
		///
		///
		///
		template        <class T>
		T* BufferImage<T>::getLinePtr(int L)
		{
#if IGN_WITH_DEBUG
			if ((L<(int)0)||(L>(int)(_size.second)))
			{
				std::ostringstream oss;
				oss << __FILE__ << " : "<<__LINE__;
				throw std::invalid_argument(oss.str());
			}
#endif
			return _data+L*_lineSpace;
		}

		///
		///
		///
		template        <class T>
		T const * BufferImage<T>::getLinePtr(int L)const
		{
#if IGN_WITH_DEBUG
			if ((L<0)||(L>(int)_size.second))
			{
				std::ostringstream oss;
				oss << __FILE__ << " : "<<__LINE__;
				throw std::invalid_argument(oss.str());
			}
#endif
			return _data+L*_lineSpace;
		}

		///
		///
		///
		template        <class T>
		T BufferImage<T>::operator () (int C,int L, int Band)const
		{
#if IGN_WITH_DEBUG
			if ((C<0)||(C>(int)_size.first) || (L<0)||(L>(int)_size.second) || (Band<0)||(Band>(int)_numBands))
			{
                std::cout<<"C L Band == "<<C<<" "<<L<<" "<<Band<<std::endl;
				std::cout<<"_size == "<<_size.first<<" "<<_size.second<<std::endl;
				std::ostringstream oss;
				oss << __FILE__ << " : "<<__LINE__;
                std::cout << oss.str();
				throw std::invalid_argument(oss.str());
			}
#endif
			return *(_data+L*_lineSpace+C*_pixelSpace+Band*_bandSpace);
		}

		///
		///
		///
		template        <class T>
		T & BufferImage<T>::operator ()(int C,int L, int Band)
		{
#if IGN_WITH_DEBUG
			if ((C<0)||(C>(int)_size.first) || (L<0)||(L>(int)_size.second) || (Band<0)||(Band>(int)_numBands))
			{
				std::cout<<"C L Band == "<<C<<" "<<L<<" "<<Band<<std::endl;
				std::cout<<"_size == "<<_size.first<<" "<<_size.second<<std::endl;
				std::ostringstream oss;
				oss << __FILE__ << " : "<<__LINE__;
                std::cout << oss.str()<<std::endl;
				throw std::invalid_argument(oss.str());
			}
#endif
			return *(_data+L*_lineSpace+C*_pixelSpace+Band*_bandSpace);
		}

		///
		///
		///
		template        <class T>
		double	BufferImage<T>::	get(int c, int l, int b) const
		{
			return (double)(*this)(c, l, b);
		}

		///
		///
		///
		template        <class T>
		void	BufferImage<T>::		set(int c, int l, int b, double val)
		{
			(*this)(c, l, b) = static_cast< T >( val );
		}

		///
		///
		///
		template        <class T>
		ImageSize BufferImage<T>::size(int deZoom)const
		{
			return _size;
		}

		///
		///
		///
		template        <class T>
		size_t						BufferImage<T>::numCols()const
		{
			return _size.first;
		}

		///
		///
		///
		template        <class T>
		size_t						BufferImage<T>::numLines()const
		{
			return _size.second;
		}

		///
		///
		///
		template        <class T>
		size_t BufferImage<T>::numBands()const
		{
			return _numBands;
		}

		///
		///
		///
		template        <class T>
		eTypePixel	BufferImage<T>::typePixel(int aDeZoom) const
		{
			return TypePixel<T>::GetType();
		}

		///
		///
		///
		template        <class T>
		bool		BufferImage<T>::isValid() const
		{
			return (_size.first>0 && _size.second>0);
		}

		///
		///
		///
		template        <class T>
		const ign::data::Object*		BufferImage<T>::metaData()const
		{
			return &_metaData;
		}

		///
		///
		///
		template        <class T>
		ign::data::Object*		BufferImage<T>::metaData()
		{
			return &_metaData;
		}

		///
		///
		///
		template        <class T>
		bool BufferImage<T>::operator == (BufferImage<T> const &other)const
		{
			if (this->size()!=other.size())
				return false;
			if (this->numBands()!=other.numBands())
				return false;
			int NC,NL;
			// this->Size(NC,NL);
			ImageSize sz = this->size();
			NC=sz.first; NL=sz.second;
			int NbBands = this->numBands();
			for(int l=0;l<NL;++l)
			{
				for(int c=0;c<NC;++c)
				{
					for(int k=0;k<NbBands;++k)
					{
						if ((*this)(c,l,k)!=other(c,l,k))
							return false;
					}
				}
			}
			return true;
		}

		///
		///
		///
		template        <class T>
		void  BufferImage<T>::save(std::string const &filename,data::Object const *  options, data::Object const *  metadata)const
		{
			bool verbose = false;
            if(verbose)std::cout << "BufferImage : save"<<std::endl;
            if (metadata)
            {
                 if(verbose)std::cout << "Utilisation des metadata donnees par l utilisateur"<<std::endl;
				 io::ImageOutput file(filename, static_cast<int>(_size.first), static_cast<int>(_size.second), static_cast<int>(_numBands), TypePixel<T>::GetType(), options, metadata);
                if (file.isValid())
                {
					file.write(0, 0, 0, static_cast<int>(_size.first), static_cast<int>(_size.second), static_cast<int>(_numBands), (void*)this->getPtr(), TypePixel<T>::GetType(), this->getPixelSpace(), this->getLineSpace(), this->getBandSpace());
                }
            }
            else
            {
                 if(verbose)std::cout << "utilisation des metadata du buffer"<<std::endl;
				 io::ImageOutput file(filename, static_cast<int>(_size.first), static_cast<int>(_size.second), static_cast<int>(_numBands), TypePixel<T>::GetType(), options, &_metaData);
                if (file.isValid())
                {
					file.write(0, 0, 0, static_cast<int>(_size.first), static_cast<int>(_size.second), static_cast<int>(_numBands), (void*)this->getPtr(), TypePixel<T>::GetType(), this->getPixelSpace(), this->getLineSpace(), this->getBandSpace());
                }
				 if(verbose)std::cout << "ecriture effectuee"<<std::endl;

            }
			if(verbose)std::cout << "[BufferImage<T>::save] done"<<std::endl;


		}

		///
		///
		///
		template        <class T>
		BufferImage<T>& BufferImage<T>::operator /= (double val)
		{
			int NC =  BufferImage<T>::_size.first;
			int NL =  BufferImage<T>::_size.second;
			T* ptrLine = _data;
			for(int l=0;l<NL;++l)
			{
				T* ptrPixel = ptrLine;
				for(int c=0;c<NC;++c)
				{
					T* ptrBand = ptrPixel;
					for(int k=0;k<(int)_numBands;++k)
					{
						(*ptrBand)=(T)((*ptrBand)/val);
						ptrBand+=_bandSpace;
					}
					ptrPixel+=_pixelSpace;
				}
				ptrLine+=_lineSpace;
			}
			return *this;
		}

		///
		///
		///
		template        <class T>
		BufferImage<T>& BufferImage<T>::operator *= (double val)
		{
			int NC =  BufferImage<T>::_size.first;
			int NL =  BufferImage<T>::_size.second;
			T* ptrLine = _data;
			for(int l=0;l<NL;++l)
			{
				T* ptrPixel = ptrLine;
				for(int c=0;c<NC;++c)
				{
					T* ptrBand = ptrPixel;
					for(int k=0;k<(int)_numBands;++k)
					{
						(*ptrBand)=(T)((*ptrBand)*val);
						ptrBand+=_bandSpace;
					}
					ptrPixel+=_pixelSpace;
				}
				ptrLine+=_lineSpace;
			}
			return *this;
		}

		///
		///
		///
		template        <class T>
		BufferImage<T>& BufferImage<T>::operator += (T val)
		{
			int NC = _size.first;
			int NL = _size.second;
			T* ptrLine = _data;
			for(int l=0;l<NL;++l)
			{
				T* ptrPixel = ptrLine;
				for(int c=0;c<NC;++c)
				{
					T* ptrBand = ptrPixel;
					for(int k=0;k<(int)_numBands;++k)
					{
						(*ptrBand)=(T)((*ptrBand)+val);
						ptrBand+=_bandSpace;
					}
					ptrPixel+=_pixelSpace;
				}
				ptrLine+=_lineSpace;
			}
			return *this;
		}

		///
		///
		///
		template        <class T>
		BufferImage<T>& BufferImage<T>::operator -= (T val)
		{
			int NC =  BufferImage<T>::_size.first;
			int NL =  BufferImage<T>::_size.second;
			T* ptrLine = _data;
			for(int l=0;l<NL;++l)
			{
				T* ptrPixel = ptrLine;
				for(int c=0;c<NC;++c)
				{
					T* ptrBand = ptrPixel;
					for(size_t k=0;k<_numBands;++k)
					{
						(*ptrBand)=(T)((*ptrBand)-val);
						ptrBand+=_bandSpace;
					}
					ptrPixel+=_pixelSpace;
				}
				ptrLine+=_lineSpace;
			}
			return *this;
		}

		///
		///
		///
		template        <class T>
		BufferImage<T>& BufferImage<T>::operator += (BufferImage<T> const &img)
		{
			int NC =  BufferImage<T>::_size.first;
			int NL =  BufferImage<T>::_size.second;
			int NC2,NL2;
			NC2 = img.numCols();
			NL2 = img.numLines();
			int nbBands2 = img.numBands();
			T* ptrLine = _data;
			const T* ptrLine2 = img.getPtr();
			for(int l=0;(l<NL)&&(l<NL2);++l)
			{
				T* ptrPixel = ptrLine;
				const T* ptrPixel2 = ptrLine2;
				for(int c=0;c<NC;++c)
				{
					if (c<NC2)
					{
						T* ptrBand = ptrPixel;
						const T* ptrBand2 = ptrPixel2;
						for(size_t k=0;k<_numBands;++k)
						{
							if ((int)k<nbBands2)
							{
								(*ptrBand)=(T)((*ptrBand)+(*ptrBand2));
								ptrBand2+=img.getBandSpace();
							}
							ptrBand+=_bandSpace;
						}
						ptrPixel2+=img.getPixelSpace();
					}
					ptrPixel+=_pixelSpace;
				}
				ptrLine+=_lineSpace;
				ptrLine2+=img.getLineSpace();
			}
			return *this;
		}

		///
		///
		///
		template        <class T>
		BufferImage<T>& BufferImage<T>::operator -= (BufferImage<T> const &img)
		{
			int NC =  BufferImage<T>::_size.first;
			int NL =  BufferImage<T>::_size.second;
			int NC2,NL2;
			NC2 = img.numCols();
			NL2 = img.numLines();
			int nbBands2 = img.numBands();
			T* ptrLine = _data;
			const T* ptrLine2 = img.getPtr();
			for(int l=0;(l<NL)&&(l<NL2);++l)
			{
				T* ptrPixel = ptrLine;
				const T* ptrPixel2 = ptrLine2;
				for(int c=0;c<NC;++c)
				{
					if (c<NC2)
					{
						T* ptrBand = ptrPixel;
						const T* ptrBand2 = ptrPixel2;
						for(int k=0;k<_numBands;++k)
						{
							if (k<nbBands2)
							{
								(*ptrBand)=(T)((*ptrBand)-(*ptrBand2));
								ptrBand2+=img.getBandSpace();
							}
							ptrBand+=_bandSpace;
						}
						ptrPixel2+=img.getPixelSpace();
					}
					ptrPixel+=_pixelSpace;
				}
				ptrLine+=_lineSpace;
			}
			return *this;
		}

		///
		///
		///
		template        <class T>
		BufferImage<T>& BufferImage<T>::operator *= (BufferImage<T> const &img)
		{
			int NC = BufferImage<T>::_size.first;
			int NL = BufferImage<T>::_size.second;
		//	int NC2,NL2;
		//	img.Size(NC2,NL2);
			int NC2 = img.numCols();
			int NL2 = img.numLines();

			int nbBands2 = img.numBands();
			T* ptrLine = _data;
			const T* ptrLine2 = img.getPtr();
			for(int l=0;(l<NL)&&(l<NL2);++l)
			{
				T* ptrPixel = ptrLine;
				const T* ptrPixel2 = ptrLine2;
				for(int c=0;c<NC;++c)
				{
					if (c<NC2)
					{
						T* ptrBand = ptrPixel;
						const T* ptrBand2 = ptrPixel2;
						for(int k=0;k<_numBands;++k)
						{
							if (k<nbBands2)
							{
								(*ptrBand)=(T)((*ptrBand)*(*ptrBand2));
								ptrBand2+=img.getBandSpace();
							}
							ptrBand+=_bandSpace;
						}
						ptrPixel2+=img.getPixelSpace();
					}
					ptrPixel+=_pixelSpace;
				}
				ptrLine+=_lineSpace;
				ptrLine2+=img.getLineSpace();
			}
			return *this;
		}

		///
		///
		///
		template        <class T>
		BufferImage<T>& BufferImage<T>::operator /= (BufferImage<T> const &img)
		{
			int NC =  BufferImage<T>::_size.first;
			int NL =  BufferImage<T>::_size.second;
			int NC2,NL2;
			//img.Size(NC2,NL2);
			NC2 = img.numCols();
			NL2 = img.numLines();
			int nbBands2 = img.numBands();
			T* ptrLine = _data;
			const T* ptrLine2 = img.getPtr();
			for(int l=0;(l<NL)&&(l<NL2);++l)
			{
				T* ptrPixel = ptrLine;
				const T* ptrPixel2 = ptrLine2;
				for(int c=0;c<NC;++c)
				{
					if (c<NC2)
					{
						T* ptrBand = ptrPixel;
						const T* ptrBand2 = ptrPixel2;
						for(int k=0;k<(int)_numBands;++k)
						{
							if (k<nbBands2)
							{
								if ((*ptrBand2)!=0)
									(*ptrBand)=(T)((*ptrBand)/(*ptrBand2));
								ptrBand2+=img.getBandSpace();
							}
							ptrBand+=_bandSpace;
						}
						ptrPixel2+=img.getPixelSpace();
					}
					ptrPixel+=_pixelSpace;
				}
				ptrLine+=_lineSpace;
				ptrLine2+=img.getLineSpace();
			}
			return *this;
		}
		
		///
		///
		///
		template        <class T>
		template        <class T2>
		BufferImage<T>& BufferImage<T>::operator += (BufferImage<T2> const &img)
		{
			for(int l=0;(l<numLines())&&(l<img.numLines());++l)
			{
				for(int c=0;(c<numCols())&&(c<img.numCols());++c)
				{
					for(int b=0;(c<numBands())&&(b<img.numBands());++b)
					{
						(*this)(c,l,b) += img(c,l,b);
					}
				}
			}
			return *this;
		}
		
		///
		///
		///
		template        <class T>
		template        <class T2>
		BufferImage<T>& BufferImage<T>::operator -= (BufferImage<T2> const &img)
		{
			for(int l=0;(l<numLines())&&(l<img.numLines());++l)
			{
				for(int c=0;(c<numCols())&&(c<img.numCols());++c)
				{
					for(int b=0;(c<numBands())&&(b<img.numBands());++b)
					{
						(*this)(c,l,b) -= img(c,l,b);
					}
				}
			}
			return *this;
		}
		
		///
		///
		///
		template        <class T>
		template        <class T2>
		BufferImage<T>& BufferImage<T>::operator *= (BufferImage<T2> const &img)
		{
			for(int l=0;(l<numLines())&&(l<img.numLines());++l)
			{
				for(int c=0;(c<numCols())&&(c<img.numCols());++c)
				{
					for(int b=0;(c<numBands())&&(b<img.numBands());++b)
					{
						(*this)(c,l,b) *= img(c,l,b);
					}
				}
			}
			return *this;
		}
		
		///
		///
		///
		template        <class T>
		template        <class T2>
		BufferImage<T>& BufferImage<T>::operator /= (BufferImage<T2> const &img)
		{
			for(int l=0;(l<numLines())&&(l<img.numLines());++l)
			{
				for(int c=0;(c<numCols())&&(c<img.numCols());++c)
				{
					for(int b=0;(c<numBands())&&(b<img.numBands());++b)
					{
						(*this)(c,l,b) /= img(c,l,b);
					}
				}
			}
			return *this;
		}
		
		///
		///
		///
		template        <class T>
		BufferImage<T>& BufferImage<T>::operator = (T val)
		{
			int NC = static_cast<int>(_size.first);
			int NL = static_cast<int>(_size.second);
			T* ptrLine = _data;
			for(int l=0;l<NL;++l)
			{
				T* ptrPixel = ptrLine;
				for(int c=0;c<NC;++c)
				{
					T* ptrBand = ptrPixel;
					for(int k=0;k<(int)_numBands;++k)
					{
						(*ptrBand)=(T)(val);
						ptrBand+=_bandSpace;
					}
					ptrPixel+=_pixelSpace;
				}
				ptrLine+=_lineSpace;
			}
			return *this;
		}

		///
		///
		///
		template        <class T>
		BufferImage<T>& BufferImage<T>::operator = (BufferImage<T> const &ori)
		{
			ImageSize taille = ori.size(1);
			int nbBands = static_cast<int>(ori.numBands());
			this->initialize(static_cast<int>(taille.first), static_cast<int>(taille.second), nbBands);
			int NC = static_cast<int>(_size.first);
			int NL = static_cast<int>(_size.second);
			T* ptrLine = _data;
			for(int l=0;l<NL;++l)
			{
				T* ptrPixel = ptrLine;
				for(int c=0;c<NC;++c)
				{
					T* ptrBand = ptrPixel;
                    for(unsigned int k=0;k<_numBands;++k)
					{
						(*ptrBand)=(T)ori(c,l,k);
						ptrBand+=_bandSpace;
					}
					ptrPixel+=_pixelSpace;
				}
				ptrLine+=_lineSpace;
			}
			return *this;
		}
		
		///
		///
		///
		template        <class T>
		template		<class T2>
		BufferImage<T>& BufferImage<T>::operator = (BufferImage<T2> const &ori)
		{
			bool verbose  = 0;
			if (verbose) std::cout<<"copie d'un buffer de taille ("<<ori.numCols()<<" x "<<ori.numLines()<<" x "<<ori.numBands() <<") avec transtypage de "<<ign::image::TypeToString(ign::image::TypePixel<T2>::GetType())<<" vers "<<ign::image::TypeToString(ign::image::TypePixel<T>::GetType())<<std::endl;
			this->initialize(ori.numCols(),ori.numLines(),ori.numBands());
			int nc = static_cast<int>(numCols());
			int nl = static_cast<int>(numLines());
			int nb = static_cast<int>(numBands());
			for(int l=0;l<nl;++l)
				for(int c=0;c<nc;++c)
					for(int b=0;b<nb;++b)
						(*this)(c,l,b) = static_cast<T>(ori(c,l,b));
			return *this;
		}
	}
}

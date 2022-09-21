#ifndef __IGN_IMAGE_IO_BUFFERIMAGE_H__
#define __IGN_IMAGE_IO_BUFFERIMAGE_H__

//a virer quand ce sera parametrable dans le CMake
#ifndef _IGN_SAFE_BUFFER_ALLOC_
#define _IGN_SAFE_BUFFER_ALLOC_
#endif

#include <complex>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <map>

#include <ignException.h>
#include <ignDataObject.h>
#include <ignImage.h>
#include <ignImageInput.h>
#include <ignImageOutput.h>


namespace ign {
    namespace image {

    	///\brief Classe proposee pour gerer les images en memoire
        template < class T >
        class BufferImage: public Image {
        public:

            ///\brief Constructeur par defaut: buffer de taille nulle*/
			BufferImage();

            ///\brief Destructeur avec liberation de la memoire*/
            virtual				~BufferImage();

            ///\brief Constructeur a partir d'un tableau deja existant*/
			BufferImage(T* data,
						ImageSize const &taille,
						int nbBands,
						int pixelSpace,
						int lineSpace,
						int bandSpace,
						bool toBeDeleted);

            ///\brief Constructeur a  partir d'un nom de fichier et d'un niveau de deZoom, booleen pour indiquer si la memoire doit etre liberee*/
			BufferImage(std::string const& fullPathName, int deZoom=1, bool del=true);

			///\brief Constructeur a  partir d'un buffer de meme type
			BufferImage(BufferImage<T> const &ori);
			
			///\brief Constructeur a  partir d'un buffer transtype
			template <class T2>
			BufferImage(BufferImage<T2> const &ori);

            ///\brief Constructeur a  partir des dimensions:
            /// \param nXSize nombre de colonnes
            /// \param nYSize nombre de lignes
			///  \param nNBands nombre de bandes
			BufferImage(int nXSize, int nYSize, int nNBands, const T &defaultValue = 0);

            ///\brief Constructeur a  partir d'un buffer memoire deja  existant
			/// attention le buffer n'est pas recopie: on utilise directement le pointeur data
			///comme le pointeur a ete fourni en entree il n'est pas sous la responsabilite de cette classe
			///et il ne sera pas desalloue dans le destructeur
			BufferImage(int nXSize, int nYSize, int nNBands, T* data, int nPixelSpace, int nLineSpace, int nBandSpace);

			/////////////METHODES HERITEES DE IMAGE/////////////
			///\brief taille du buffer
            ImageSize						size(int aDeZoom=1)const;
			///\brief nombre de colonnes
			size_t							numCols()const;
			///\brief nombre de lignes
			size_t							numLines()const;
			///\brief type de pixels
			eTypePixel						typePixel(int aDeZoom = 1) const;
			///\brief indique s'i l'image est valide
			bool							isValid() const;
            ///\brief Nombre de bandes
            size_t							numBands()const;
			///\brief renvoie les metadonnees attachees a l'image (const)
			const ign::data::Object*		metaData()const;
			///\brief renvoie les metadonnees attachees a l'image
			ign::data::Object*				metaData();

			///\brief	indique si ce type d'image est en memoire en plusieurs blocs, ou non
			bool							isTiled() const;
			/////////////METHODES PROPRES A LA CLASSE/////////////

			///\brief Initialisation a  partir d'un nom de fichier et d'un niveau de deZoom*/
            void							initialize(std::string const& fullPathName, int deZoom=1, bool del=true, bool useExtension=false) ;
            ///\brief Modifier la taille du buffer: attention les donnees sont reinitialisees
            void							initialize(int NRows, int NLines, int NBands, const T& defaultValue);
            ///\brief Modifier la taille du buffer: attention les donnees ne sont pas reinitialisee
            void							initialize(int NRows, int NLines, int NBands);
			///\brief Sauvegarde de l'image en utilisant une ImageOutput
            void							save(std::string const &filename, data::Object const *  options = NULL, data::Object const *  metadata = NULL)const;
            ///\brief Extraction d'une sous image
            void							crop(BufferImage<T> const &src, int x1, int y1, int x2, int y2, int clip=1);
			///\brief Rotation d'une image de 90 deg
            void							rotate(BufferImage<T> const &src);
			///\brief Extraction d'une sous image avec rotation de 90 deg
			void							cropandrotate(BufferImage<T> const &src, int x1, int y1, int x2, int y2, int clip=1);

			bool							isReadable()const;
			bool							isWritable()const;
			bool							isWritable(std::string nomImage)const;
			bool							isInRAM()	const;


			///\brief	lecture dans un @param buffer de destination (si image readable)
			void							read(int nXOff, int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, int nDeZoom, void *buffer, eTypePixel eBufferType, int nPixelSpace, int nLineSpace, int nBandSpace)const;
			///\brief	ecriture dans un buffer (si image writable)
			void							write(int nXOff,int nYOff, int nBand0, int nXSize, int nYSize, int nNBands, void *buffer, eTypePixel eBufferType,int nPixelSpace, int nLineSpace, int nBandSpace);
			///\brief	recuperation d'un pixel de l'image this
			inline double					get(int c, int l, int b) const;
			///\brief	mis a jour d'un pixel de l'image this
			void							set(int c, int l, int b, double val);

			///ACCES A LA STRUCTURE MEMOIRE
            ///\brief Pointeur vers le debut du bloc memoire
            T*								getPtr();
            ///\brief Pointeur vers le debut du bloc memoire
            T const *						getPtr()const;
            ///\brief decalage memoire pour passer d'un pixel au suivant (typiquement NBands)*/
            int								getPixelSpace()const;
            ///\brief decalage memoire pour passer d'une ligne a  la suivante (typiquement NCol*NBands)*/
            int								getLineSpace()const;
            ///\brief decalage memoire pour passer d'un canal au suivant (typiquement 1)*/
            int								getBandSpace()const;
            ///\brief pointeur vers le debut de la ligne L
            T*								getLinePtr(int L);
            ///\brief pointeur vers le debut de la ligne L
            T const *						getLinePtr(int L)const;



			///OPERATORS
            ///\brief acces a  un pixel*/
            T								operator () (int C,int L, int Band=0)const;

            ///\brief acces a  un pixel
            T &								operator ()(int C,int L, int Band=0);

			///\brief Operateur de comparaison (taille et pixel a pixel)
            bool							operator ==(BufferImage<T> const &other)const;

            ///\brief Operation sur l'ensemble des pixels
            BufferImage<T>&					operator /= (double val);

            ///\brief Operation sur l'ensemble des pixels
            BufferImage<T>&					operator *= (double val);

            ///\brief Operation sur l'ensemble des pixels
            BufferImage<T>&					operator += (T val);

            ///\brief Operation sur l'ensemble des pixels
            BufferImage<T>&					operator -= (T val);

            ///\brief Operation sur l'ensemble des pixels
            BufferImage<T>&					operator += (BufferImage<T> const &img);

            ///\brief Operation sur l'ensemble des pixels
            BufferImage<T>&					operator -= (BufferImage<T> const &img);

            ///\brief Operation sur l'ensemble des pixels
            BufferImage<T>&					operator *= (BufferImage<T> const &img);

            ///\brief Operation sur l'ensemble des pixels
            BufferImage<T>&					operator /= (BufferImage<T> const &img);
			
			///\brief Operation sur l'ensemble des pixels avec transtypage
			template <class T2>
			BufferImage<T>&					operator += (BufferImage<T2> const &img);
			
			///\brief Operation sur l'ensemble des pixels avec transtypage
			template <class T2>
			BufferImage<T>&					operator -= (BufferImage<T2> const &img);
			
			///\brief Operation sur l'ensemble des pixels avec transtypage
			template <class T2>
			BufferImage<T>&					operator *= (BufferImage<T2> const &img);
			
			///\brief Operation sur l'ensemble des pixels avec transtypage
			template <class T2>
			BufferImage<T>&					operator /= (BufferImage<T2> const &img);
			
            ///\brief Operation sur l'ensemble des pixels
            BufferImage<T>&					operator = (T val);

            ///\brief Recopie d'une image
            BufferImage<T>&					operator = (BufferImage<T> const &ori);
			
			///\brief Recopie d'une image avec gestion du transtypage
			template <class T2>
			BufferImage<T>&					operator = (BufferImage<T2> const &ori);

        protected:
			///\brief pointeurs vers les pixels
            T*							_data;
			///\brief taille de l'image en memoire
            ImageSize					_size;
			///\brief nombre de bandes de l'image
            size_t						_numBands;
			///\brief entrelacement des pixels
            int							_pixelSpace;
			///\brief entrelacement des lignes
            int							_lineSpace;
			///\brief entrelacement des bandes
            int							_bandSpace;
			///\brief
            bool						_delete;
			///\brief					metadonnees attachees a l'image
			ign::data::Object			_metaData;
        };


	}
}

#include <ignBufferImage_code.h>

#endif

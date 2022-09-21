// Passage en numerotation contigue
template<class T>
void renumerotation(BufferImage<T> & img) {

	int idx = 0;
	std::map<T, T> LUT;
	T pixelValue = 0;
	for (size_t i = 0; i < img.numLines(); i++) {
		for (size_t j = 0; j < img.numCols(); j++) {
			for (size_t k = 0; k < img.numBands(); k++) {
				pixelValue = (T) (img(j, i, k));

				typename std::map<T, T>::iterator itLUT = LUT.find(pixelValue);
				if (itLUT == LUT.end()) {
					T new_label = idx;
					LUT[pixelValue] = new_label;
					img.set(j, i, k, new_label);

					idx = idx + 1;
				} else {
					img.set(j, i, k, (*itLUT).second);
				}
			}
		}
	}
}

template<class T>
void renumerotationSpecific(BufferImage<T> & img, std::map<T,T> LUT)	{
	T pixelValue = 0;
    for (size_t i = 0; i < img.numLines(); i++) {
		for (size_t j = 0; j < img.numCols(); j++) {
			for (size_t k = 0; k < img.numBands(); k++) {
				pixelValue = (T) (img(j, i, k));
				typename std::map<T, T>::iterator itLUT = LUT.find(pixelValue);
				if (itLUT != LUT.end()) {
					img.set(j, i, k, (*itLUT).second);
				}
			}
		}
	}
}

template<class T>
void renumerotationSpecific2(BufferImage<T> & img, std::map<T, std::set< std::pair<size_t, T> >* > &LUT)	{
	T pixelValue = 0;
	for (size_t i = 0; i < img.numLines(); i++) {
		for (size_t j = 0; j < img.numCols(); j++) {
			for (size_t k = 0; k < img.numBands(); k++) {
				pixelValue = (T) (img(j, i, k));
				typename std::map<T, std::set< std::pair<size_t, T> >* >::iterator itLUT = LUT.find(pixelValue);
					if (itLUT != LUT.end()) {
					typename std::set< std::pair<size_t, T> >* mySet = (*itLUT).second;
					if (mySet->empty()) {
						IGN_THROW_EXCEPTION("SHOULD NOT HAPPEND -> ign/image/segment/utils/numeroteRegion.h -> renumerotationSpecific2 -> Empty Set !");
					}
					std::pair<size_t, T> pair = *(mySet->rbegin());
					T newValue = pair.second;
					img.set(j, i, k, newValue);
				}
			}
		}
	}
}

//Cette version est plus gourmande en memoire que la precedente, car elle implique de recopier l'image, ce qui pourrait poser probleme avec de tres grandes images
template <class T> inline void renumerotation_composante_connexe( int& labelmax, BufferImage<T> & imagelabel/*, std::vector<Region> & regions_classif*/, const bool ignore0, const int connexite){

	BufferImage<T> imagelabelconn(imagelabel.numCols(), imagelabel.numLines(), 1, 0);
	std::set<T2D<int> > nouveaux;
	std::set<T2D<int> >::iterator its;
	int imax=imagelabel.numCols()-1;
	int jmax=imagelabel.numLines()-1;
	T label_region, label_regiontmp;

	int maxByLimit = std::numeric_limits<T>::max();

	// progress bar.
	size_t len = imax * jmax;
	boost::progress_display progressBar(len);

	for(size_t i=0;i<imagelabel.numCols();i++){
		for(size_t j=0;j<imagelabel.numLines();j++){
			++progressBar;
			if(imagelabelconn(i,j)!=0){continue;}//si le pixel a deja ete visite, inutile de s'y attarder. Remarque : il serait mieux d'avoir une valeur différente de 0 pour les zones non visitees (éviterait les problèmes dans le cas ou l'on aurait une classse de label de 0 que l'on souhaite prendre en compte.
			label_region=imagelabel(i,j);
			if(label_region==0 && ignore0){continue;}
			labelmax++;

			// Gestion d'erreur, si le label depasse le max de la valeur de l'entier pour ce template.
			if (labelmax > maxByLimit) {
				std::stringstream error;
				error << "Erreur lors de la renumerotation : label atteint : " << labelmax << " superieurs a " << maxByLimit;

				// On lance une exception pour ne pas tomber dans une boucle infinie.
				IGN_THROW_EXCEPTION(error.str());
			}
			//	  std::cout << "labelMax : " << labelmax << "i : " << i << " ; j : " << j << std::endl;
			nouveaux.insert(T2D<int> (i,j));
			while(nouveaux.size()!=0){//tant qu'on trouve de nouveaux pixels appartenant a la zone courante, on continue
				std::set<T2D<int> > nouveaux_tmp;
				for(its=nouveaux.begin();its!=nouveaux.end();++its){
					int I=(*its).x();
					int J=(*its).y();
					//Recherche parmi les voisins des nouveaux pixels listes lors de l'iteration precedente
					for(int c=I-1;c<I+2;c++){
						if(c<0 || c>imax){continue;}
						for(int l=J-1;l<J+2;l++){
							if(l<0 || l>jmax){continue;}
							if(connexite==4 && I!=c && J!=l){continue;}//Pb de connexites des zones
							if(imagelabelconn(c,l)!=0){continue;}
							label_regiontmp=imagelabel(c,l);
							if(label_regiontmp==label_region){
								imagelabelconn(c,l)=labelmax;
								nouveaux_tmp.insert(T2D<int> (c,l));
								//		region.add(c,l);
							}
						}
					}
				}
				nouveaux.clear();
				nouveaux=nouveaux_tmp;
			}

			nouveaux.clear();
		}
	}
	imagelabelconn -= 1;
	imagelabel=imagelabelconn;
}

// Passage en numerotation contigue
template<class T>
BufferImage<unsigned char>* renumerotationOneChannelToRGB(BufferImage<T> & img) {
	unsigned char currentHue = 2;
	BufferImage<unsigned char> * res = new BufferImage<unsigned char>();
    res->initialize(img.numCols(), img.numLines(), 3);
	std::map<T, RgbColor*> LUT;
    T pixelValue = 0;
	for (size_t i = 0; i < img.numLines(); i++) {
		for (size_t j = 0; j < img.numCols(); j++) {
			RgbColor* color;
			pixelValue = (T) (img(j, i, 0));
			typename std::map<T, RgbColor*>::iterator itLUT = LUT.find(pixelValue);
			if (itLUT == LUT.end()) {
				HsvColor hsv;
				hsv.s = 200;
				hsv.v = 200;
				hsv.h = (unsigned char) currentHue;
				color = HsvToRgb(hsv);
				currentHue += 5;
				if (currentHue >= 255)
					currentHue = 2;
				LUT[pixelValue] = color;
			} else {
				color = (*itLUT).second;
			}
			res->set(j, i, 0, color->r);
			res->set(j, i, 1, color->g);
			res->set(j, i, 2, color->b);
		}
	}
	return res;
}

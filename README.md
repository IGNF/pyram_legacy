# pyram_legacy

Pyram legacy is an IGN implementation of *Scale-set Image Analysis, hierarchical image segmentation* from L. Guigues phd at IGN Matis laboratory 

## Doc
A more complet documentation (WIP) could be found on [github pages](https://ignf.github.io/pyram_legacy/intro.html)

## License
[![License : AGPL v3](https://img.shields.io/badge/license-AGPL3-blue.svg)](https://github.com/IGNF/pyram_legacy/blob/master/LICENSE)

## Bibliography

- [Scale-sets Image Analysis](https://hal.science/hal-00705364/),  Laurent Guigues , Jean Pierre Cocquerez , Hervé Le Men - 2066
- [Modèles Multi-Échelles pour la Segmentation d'Images](http://recherche.ign.fr/labos/matis/pdf/theses/these_guigues.pdf) Laurent Guigues 2003

## Related projects
- [hierarchy labellisation](https://github.com/IGNF/hierarchy_labellisation), a POC made by [EPITA](https://www.epita.fr/en/) students to test a possible reimplementation of Pyram algorithm in Rust and executed client side with wasm.
- [Higra: Hierarchical Graph Analysis](https://github.com/higra/Higra) Higra is a C++/Python library for efficient sparse graph analysis with a special focus on hierarchical methods. A graph could also be an image. A equivalent of pyram algorithms is provided by higra methods [hierarchy_to_optimal_MumfordShah_energy_cut_hierarchy](https://higra.readthedocs.io/en/stable/python/tree_energy_optimization.html#higra.hierarchy_to_optimal_MumfordShah_energy_cut_hierarchy)
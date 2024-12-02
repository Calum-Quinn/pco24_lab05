<div align="justify" style="margin-right:25px;margin-left:25px">

# Laboratoire 5 : Quicksort multithreadé <!-- omit from toc -->

## Etudiants <!-- omit from toc -->

- Calum Quinn
- Urs Behrmann

# Table des matières

- [Table des matières](#table-des-matières)
- [Introduction](#introduction)
- [Conception](#conception)
  - [Choix d'implémentation](#choix-dimplémentation)
- [Tests](#tests)
  - [Tests de l'algorithme de tri](#tests-de-lalgorithme-de-tri)
  - [Tests de concurrence](#tests-de-concurrence)
- [Conclusion](#conclusion)

# Introduction

Ce laboratoire a pour objectif d'explorer les principes de la programmation concurrente à travers l'implémentation de l'algorithme de tri rapide (Quicksort) en version multithreadée. Ce projet met en pratique la gestion des threads et la synchronisation via un moniteur de Mesa, permettant une répartition efficace des tâches de tri. Il s'inscrit dans une approche pédagogique visant à développer des compétences en conception et réalisation de programmes parallèles, tout en respectant des contraintes d'optimisation et de gestion des ressources.

Les étapes du travail incluent la conception d'une solution capable d'exploiter un nombre défini de threads, l'intégration de mécanismes de synchronisation, et la vérification de l'efficacité à travers des tests et des benchmarks. Une attention particulière sera portée à la documentation du processus et des choix d'implémentation.

# Conception

Pour notre algorithme, nous nous sommes inspirés du modèle producteur-consommateur, où le producteur joue également le rôle de consommateur.

Le thread principal crée les threads de travail qui effectueront le tri. Ces threads attendent leur tour grâce à un moniteur de Mesa. Le thread principal ajoute la première tâche dans une pile FIFO et réveille un thread pour qu'il commence à travailler. Enfin, le thread principal attend la fin du travail global à l'aide du même moniteur.

Les threads de travail suivent tous le même fonctionnement. Ils attendent un signal du moniteur de Mesa pour commencer leur tâche. Une fois réveillés, ils prennent la première tâche disponible dans la pile FIFO et entrent dans la fonction de tri.

Dans la fonction de tri, le thread vérifie si d'autres threads peuvent être réveillés. Si c'est possible, le travail restant est divisé en deux, et les deux nouvelles tâches sont ajoutées à la pile FIFO. Un autre thread est alors réveillé pour prendre en charge l'une des tâches. Si aucun thread n'est disponible, le thread en cours s'occupe directement de la tâche reçue en exécutant l'algorithme Quicksort.

Lorsqu'un thread termine son travail ou divise la tâche, il se retire de la liste des threads actifs et vérifie si le travail global est terminé. Si c'est le cas, il réveille le thread principal, qui pourra ordonner l'arrêt des threads. Si le travail n'est pas terminé, le thread prend une nouvelle tâche dans la pile FIFO et recommence le processus.

## Choix d'implémentation



# Tests

On a fait 2 types de tests pour vérifier le bon fonctionnement de notre programme. Le premier groupe sont les tests en rapport avec le bon fonctionnement de l'algorithme de tri. Le deuxième groupe de tests sont en rapport avec la gestion des threads, la synchronisation et la concurrence.

## Tests de l'algorithme de tri

Pour vérifier que notre algorithme de tri fonctionne correctement, on a fait les tests suivants :

- Tests "simple" de tri avec des tableaux de tailles variées allant jusqu'à `std::numeric_limits<int>::max() / 100` éléments.
- Tests de tri avec des tableaux déjà triés, inversés, avec des doublons ou avec seulement des éléments identiques.

## Tests de concurrence

Pour vérifier que notre programme fonctionne correctement en concurrence, on a fait les tests suivants :

- on a lancé les tests de tri avec 0, 1000 et ´std::numeric_limits<int>::max()´ threads pour vérifier que le programme fonctionne correctement avec un nombre variable de threads.
- on a lancé des tests avec des tableaux plus petits que le nombre de threads pour vérifier que le programme fonctionne correctement avec des threads inactifs.

# Conclusion


</div>
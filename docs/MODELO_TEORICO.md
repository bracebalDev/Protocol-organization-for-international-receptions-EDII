# Modelo Teórico: Teoría de Grafos y Optimización de Protocolo

**Universidad de Carabobo**  
**Facultad Experimental de Ciencias y Tecnología (FACYT)**  
**Departamento de Computación**  
**Unidad Académica de Elementos Discretos II**  
**Autor:** Brayan Ceballos (`bracebalDev`)  

---

## 1. Definición Formal del Problema

El Departamento de la Asamblea General para la Gestión de Conferencias (**DAGGC**) de la **Organización de las Naciones Unidas (ONU)** organiza recepciones diplomáticas anuales. Debido a conflictos geopolíticos y relaciones hostiles, se exige que mandatarios incompatibles **no compartan la misma mesa**, minimizando el total de mesas requeridas y nivelando la cantidad de delegados por mesa.

### Modelado mediante Grafos
El problema se formaliza mediante un **grafo simple no dirigido** $G = (V, E)$:

* **Conjunto de Vértices $V$:**  
  $$V = \{v_1, v_2, \dots, v_n\}$$  
  Representa el conjunto de los $n$ delegados invitados ($1 \le i \le n$).

* **Conjunto de Aristas $E$:**  
  $$E \subseteq \{\{u, v\} \mid u, v \in V, u \neq v\}$$  
  Una arista $e = \{u, v\} \in E$ denota que los delegados $u$ y $v$ poseen una incompatibilidad diplomática y no pueden asignarse a la misma mesa.

* **Matriz de Adyacencia $A \in \{0, 1\}^{n \times n}$:**  
  $$A_{ij} = \begin{cases} 1 & \text{si } \{i, j\} \in E \\ 0 & \text{en caso contrario} \end{cases}$$  
  Dado que $G$ es no dirigido y sin lazos, $A$ es simétrica ($A_{ij} = A_{ji}$) y su diagonal principal es nula ($A_{ii} = 0$).

---

## 2. Mapeo Teórico de los Requerimientos

### Requerimiento 1: Consulta de Incompatibilidad Diplomática
* **Concepto Teórico:** Adyacencia de vértices en $G$.
* **Operación:**  
  $$\text{incompatibles}(u, v) \iff A_{u-1, v-1} = 1$$
* **Complejidad Temporal:** $\mathcal{O}(1)$ sobre la matriz de adyacencia.

---

### Requerimiento 2: Delegado(s) más Conflictivo(s)
* **Concepto Teórico:** Grado de un vértice $\text{deg}(v)$ y Grado Máximo del Grafo $\Delta(G)$.
* **Fórmula:**  
  $$\text{deg}(v) = \sum_{u \in V} A_{v-1, u-1}$$
  $$\Delta(G) = \max_{v \in V} \text{deg}(v)$$
  $$V_{\text{max}} = \{v \in V \mid \text{deg}(v) = \Delta(G)\}$$
* **Complejidad Temporal:** $\mathcal{O}(|V|)$ con caché de grados precalculada.

---

### Requerimiento 3: Delegados sin Incompatibilidades
* **Concepto Teórico:** Vértices Aislados / Vértices de Grado Nulo.
* **Fórmula:**  
  $$I = \{v \in V \mid \text{deg}(v) = 0\}$$
* **Complejidad Temporal:** $\mathcal{O}(|V|)$.

---

### Requerimiento 4: Distribución Óptima de Mesas con Balanceo
* **Concepto Teórico:** 
  1. **Coloración de Vértices** (Vertex Coloring) y **Partición en Conjuntos Independientes**:
     Una $k$-coloración válida de $G$ es una asignación $c: V \to \{1, \dots, k\}$ tal que:
     $$\forall \{u, v\} \in E \implies c(u) \neq c(v)$$
     Cada clase de color $S_j = \{v \in V \mid c(v) = j\}$ es un **Conjunto Independiente** (mesa).
     El número mínimo de mesas requeridas corresponde al **Número Cromático** $\chi(G)$.

  2. **Subgrafo de Conflictos $G' = G[V \setminus I]$:**
     Los vértices aislados $I$ no tienen restricciones de adyacencia ($\forall w \in I, \forall v \in V, \{w, v\} \notin E$). Por tanto, pueden insertarse en cualquier mesa sin violar la propiedad de conjunto independiente.

  3. **Algoritmo de Partición y Balanceo Voraz:**
     * **Paso 1 (Aislamiento):** Identificar $I = \{v \in V \mid \text{deg}(v) = 0\}$ y $V' = V \setminus I$.
     * **Paso 2 (Coloración de $G'$):** Extraer conjuntos independientes maximales $S_1, S_2, \dots, S_k$ cubriendo $V'$.
     * **Paso 3 (Balanceo de Carga):** Para cada vértice libre $w \in I$, asignarlo a la mesa $S^*$ con cardinalidad mínima:
       $$S^* = \arg\min_{1 \le j \le k} |S_j|$$
       $$S^* \leftarrow S^* \cup \{w\}$$

* **Garantía de Correctitud:**
  Dado que ningún $w \in I$ tiene aristas incidentes, cada conjunto $S_j$ resultante sigue siendo un conjunto independiente válido en $G$.

---

## 3. Demostración de Correctitud del Caso Oficial

Para el archivo oficial `Convencion.in` ($n=25, m=22$):
* $\Delta(G) = 4$ con delegados $\{1, 2, 8, 17\}$.
* Vértices aislados $I = \{5, 10, 11, 21\}$ ($|I| = 4$).
* Subgrafo $G'$ contiene 3-cliques ($K_3$) como $\{17, 18, 23\}$ y $\{17, 22, 23\}$, por lo que $\chi(G') \ge 3$.
* La coloración genera 3 conjuntos independientes:
  * Mesa 1: $9$ delegados base
  * Mesa 2: $10$ delegados base
  * Mesa 3: $2$ delegados base
* El balanceo distribuye los $4$ delegados de $I$ en la Mesa 3 ($2 + 4 = 6$), logrando una distribución equitativa ($9, 10, 6$) en solo 3 mesas.

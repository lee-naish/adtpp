set exrc
map !! :w
map !! :w
map !! :w
map !! :w
set wrapmargin=8
set wrapscan
map !w :set wrapmargin=0 nowrapscan
map q :wq
map N :w
map z :w
map %$ :'c,.s/^/\/\/ /
map %^ :'c,.s/^\/\/ //
map %t :%s/        /	/g
map , Ea,'
map !d :.! date
map !u :.! date -u
map !q Bi``Ea''
map !t Bi\texttt{Ea}
map !v Bi\verb@Ea@
map !c Bi\cite{Ea}
map !b bi\textbf{ea}
map !i bi\emph{ea}
map !e :s/^.*$/&
map !r :s/^.*$/&
map !f !{fmt
map !h1 0i\section{$a}
map !h2 0i\subsection{$a}
map !h3 0i\subsubsection{$a}
ab	veR \begin{verbatim}
ab	itE \begin{itemize}
ab	enU \begin{enumerate}
ab	taB \begin{tabular}{lr}
ab	fiG \begin{figure}
ab	bwS \begin{bwslide}
ab	iT  \item 
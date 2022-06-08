set exrc
set wrapmargin=8
set wrapscan
map !w :set wrapmargin=0 nowrapscan
map q :wq
map N :w
map z :w
map !! :w
map %$ :'c,.s/^.*$/<!-- & -->/
map %^ :'c,.s/^<!-- \(.*\) -->$/\1/
map %t :%s/        /	/g
map , Ea,'
map !q Bi"Ea"
map !d :.! date
map !u :.! date -u
map !b bi<strong>ea</strong>
map !i bi<em>ea</em>
map !p Bi<code>Ea</code>
map !c Bi<code>Ea</code>
map !v Bi<pre>Ea</pre>
map !h1 0i<h1>$a</h1>
map !h2 0i<h2>$a</h2>
map !h3 0i<h3>$a</h3>
map !e :s/^.*$/&
map !r :s/^.*$/&
map !< :'a,'bs/</\&lt;/g
ab	htM http://www.cs.mu.oz.au/
ab	aH <a href="">
ab	veR <pre>
ab	taB <table>
ab	itE <ul>
ab	enU <ol>
ab	iT <li> 
ab	dL <dl>
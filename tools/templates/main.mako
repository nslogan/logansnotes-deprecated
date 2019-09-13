<!doctype html>
<html lang="">

<head>
	<meta charset="utf-8">
	<title>${page_title}</title>
	<meta name="description" content="">
	<meta name="viewport" content="width=device-width, initial-scale=1">

	## <link rel="manifest" href="site.webmanifest">
	## <link rel="apple-touch-icon" href="icon.png">
	<!-- Place favicon.ico in the root directory -->

	## <link rel="stylesheet" href="css/normalize.css">
	## <link rel="stylesheet" href="css/main.css">
	<link href="../../assets/css/primer.css" rel="stylesheet" />
	## TODO: This needs to be an absolute reference - that means passing in the site URL (and that means differentiating between production and development builds where I will be deploying to a local server or production server)
	<link href="../css/highlight.css" rel="stylesheet" />

	## <meta name="theme-color" content="#fafafa">
</head>

<body>
	<!--[if IE]>
		<p class="browserupgrade">You are using an <strong>outdated</strong> browser. Please <a href="https://browsehappy.com/">upgrade your browser</a> to improve your experience and security.</p>
	<![endif]-->

	<div class="Header">
		<div class="Header-item">
			<strong>Logan's Notes</strong>
		</div>
	</div>

	<div class="container-lg clearfix px-3">
		<article class="markdown-body pt-5 px-5">
			${page_content}
		</article>
	</div>

	<div class="footer container-lg width-full clearfix px-3">
		## <div class="pt-6 pb-2 border-top border-gray-light">
		<div class="pt-6 pb-2 mt-6 f6 text-gray border-top border-gray-light">
			<ul class="list-style-none d-flex flex-wrap">
				<li class="mr-3">2019</li>
				<li class="mr-3">Logan Smith</li>
				<li class="mr-3"><a href="https://logansmith.org/">https://logansmith.org/</a></li>
			</ul>
		</div>
		<div class="pb-6"></div>
	</div>

	## <script src="js/vendor/modernizr-{{MODERNIZR_VERSION}}.min.js"></script>
	## <script src="https://code.jquery.com/jquery-{{JQUERY_VERSION}}.min.js" integrity="{{JQUERY_SRI_HASH}}" crossorigin="anonymous"></script>
	## <script>window.jQuery || document.write('<script src="js/vendor/jquery-{{JQUERY_VERSION}}.min.js"><\/script>')</script>
	## <script src="js/plugins.js"></script>
	## <script src="js/main.js"></script>

	<!-- Google Analytics: change UA-XXXXX-Y to be your site's ID. -->
	## <script>
	## 	window.ga = function () { ga.q.push(arguments) }; ga.q = []; ga.l = +new Date;
	## 	ga('create', 'UA-XXXXX-Y', 'auto'); ga('set','transport','beacon'); ga('send', 'pageview')
	## </script>
	## <script src="https://www.google-analytics.com/analytics.js" async></script>
</body>

</html>
var path = require('path');
var webpack = require('webpack');
var HtmlWebpackPlugin = require('html-webpack-plugin');

// Phaser webpack config
var phaserModule = path.join(__dirname, '/node_modules/phaser-ce/');
var phaser = path.join(phaserModule, 'build/custom/phaser-split.js');
var pixi = path.join(phaserModule, 'build/custom/pixi.js');
var p2 = path.join(phaserModule, 'build/custom/p2.js');

const BUILD_DIR = path.resolve(__dirname, 'build');

module.exports = {
    entry: {
        app: [
            'babel-polyfill',
            path.resolve(__dirname, 'src/main.js')
        ],
        vendor: ['pixi', 'p2', 'phaser', 'webfontloader']
    },
    devtool: 'cheap-source-map',
    output: {
        pathinfo: true,
        path: BUILD_DIR,
        publicPath: './',
        filename: 'bundle.js'
    },
    devtool: 'source-map',

    devServer: {
        inline: true,

        contentBase: [BUILD_DIR],
        // match the output path

        publicPath: './',
        // match the output `publicPath`

        port: 3000, 

    },
    
    watch: true,
    module: {
        rules: [
            {
                test: /\.js$/,
                use: ['babel-loader'],
                include: path.join(__dirname, 'src')
            },
            {
                test: /pixi\.js/,
                use: ['expose-loader?PIXI']
            },
            {
                test: /phaser-split\.js$/,
                use: ['expose-loader?Phaser']
            },
            {
                test: /p2\.js/,
                use: ['expose-loader?p2']
            }
        ]
    },
    resolve: {
        alias: {
            'phaser': phaser,
            'pixi': pixi,
            'p2': p2
        }
    },
    plugins: [
        new webpack.optimize.CommonsChunkPlugin({ name: 'vendor'/* chunkName= */, filename: 'vendor.bundle.js'/* filename= */ }),
        new HtmlWebpackPlugin({
            filename: '../build/index.html',
            template: './src/index.html',
            chunks: ['vendor', 'app'],
            chunksSortMode: 'manual',
            minify: {
                removeAttributeQuotes: false,
                collapseWhitespace: false,
                html5: false,
                minifyCSS: false,
                minifyJS: false,
                minifyURLs: false,
                removeComments: false,
                removeEmptyAttributes: false
            },
            hash: false
        }),
        // new BrowserSyncPlugin({
        //     host: process.env.IP || 'localhost',
        //     port: process.env.PORT || 3000,
        //     server: {
        //         baseDir: ['./', './build']
        //     }
        // })
    ],
}
import * as mj from 'mathjax-node';
import * as svgexport from 'svgexport';
import { writeFile, unlink } from 'fs';

export default class mathjax {
    constructor(c: number) {
        mj.config({
            MathJax: {
                displayMessages: true,
                displayErrors: true,
                undefinedCharError: true,
                extensions: '',
                MathJax: {}
            }
        });

        this.counter = c;
    }

    public out(s: string, dir: string, positionForDependence: [number, number][]): Promise<[string, [number, number]]> {
        return new Promise<[string, [number, number]]>((resolve: (fname?: [string, [number, number]]) => void, reject: (reason?: any) => void) => {
            mj.typeset({
                math: s,
                format: "TeX",
                svg: true,
                scale: 1
            }, (data: any) => {
                const erCall: (er: any) => void = (er: any) => { reject(er); };              
                if (data.errors) erCall(data.errors);
                
                const counter: number = ++this.counter;
                const svgname: string = dir + counter.toString() + '.svg';
                const pngname: string = dir + counter.toString() + '.png';
                const position: [number, number] = positionForDependence[counter - 1];
            
                this.writeFile(svgname, data.svg + '\n')
                    .then((_: string) => {
                        console.log('generating complete svg: ' + svgname + ' export to: ' + pngname + '...'); 
                        this.svgExport({ "input": [svgname], "output": [pngname] })
                            .then((_: string) => {
                                console.log('exporting complete png: ' + pngname);
                                unlink(svgname, erCall);
                                console.log('cleanup svg: ' + svgname);
                                resolve([pngname, position]);
                            })
                            .catch(erCall);
                    })
                    .catch(erCall);
            })
        });
    }

    private writeFile(file: string, contents: string): Promise<string> {
        return new Promise<string>((resolve: (value?: string) => void, reject: (reason?: any) => void) => {
            writeFile(file, contents, (err: Error) => {
                if (err) {
                    reject(err);
                    return;
                }
                resolve("Success");
            });
        });
    }

    private svgExport(renderSettings: Object): Promise<string> {
        return new Promise<string>((resolve: (value?: string) => void, reject: (reason?: any) => void) => {
            svgexport.render(renderSettings, (err: Error) => {
                if (err) {
                    reject(err);
                    return;
                }
                resolve("Success");
            });
        });
    }

    private counter: number;
}
